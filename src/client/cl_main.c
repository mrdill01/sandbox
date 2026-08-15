#include "client.h"
#include "../shared/quark.h"
#include "../shared/net.h"

void cl_init(quark_t* quark, client_t* client) {
    info(quark, "cl_init()...");
    client->host = NULL;
    client->peer = NULL;

    client->nbuffer = 0;

    client->has_sent_name = false;
    info(quark, "client initialized!");
}

void cl_connect(quark_t* quark, client_t* client, const char* ip, int port) {
    if (cl_is_connected(quark, client)) {
        cl_disconnect(quark, client);
    }

    client->host = enet_host_create(NULL, 1, NET_CHANNELS, 0, 0);
    if (!client->host) {
        error(quark, "[client] failed to create host (enet_host_create failed)");
        return;
    }

    ENetAddress address = {0};
    enet_address_set_host(&address, ip);
    address.port = NET_PORT;

    info(quark, "[client] connecting to %s:%d...", ip, port);

    client->peer = enet_host_connect(client->host, &address, NET_CHANNELS, 0);
    if (!client->peer) {
        error(quark, "[client] failed to connect! (enet_host_connect failed)");
        return;
    }

    info(quark, "[client] connected!");
}

void cl_disconnect(quark_t* quark, client_t* client) {
    if (!cl_is_connected(quark, client)) return;

    info(quark, "[client] disconnecting...");
    enet_peer_reset(client->peer);
    enet_host_destroy(client->host);
    client->peer = NULL;
    client->host = NULL;
    info(quark, "[client] disconnected!");
}

void cl_tick(quark_t* quark, client_t* client) {
    if (!client->host || !client->peer) return;

    cl_send(quark, client);

    ENetEvent event;
    while (enet_host_service(client->host, &event, NET_TIMEOUT_MSEC) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_NONE: break;
        case ENET_EVENT_TYPE_CONNECT: break;
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            info(quark, "[client] lost connection");
            quark->ui_state = UI_STATE_MAIN_MENU;
            quark->renderer.ui.show_msgbox = true;
            quark->renderer.ui.msgbox_message = "lost connection";
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            cl_recv(quark, client, event.packet);    
            break;
        }
        }
    }
}

void cl_send(quark_t* quark, client_t* client) {
    if (!cl_is_connected(quark, client)) return;

    if (!client->has_sent_name) {
        uint8_t msg[NET_MAX_PLAYER_NAME_LEN + 1];
        sprintf((char*)msg, "%s", cl_name.string);

        cl_write_byte(quark, client, CSV_SET_NAME);
        cl_write_bytes(quark, client, msg, strlen((char*)msg) + 1);
        client->has_sent_name = true;
        info(quark, "[client] sending name...");
    }

    if (client->nbuffer == 0) {
        cl_write_byte(quark, client, CSV_NOTHING);
        cl_write_byte(quark, client, '\0');
    }

    ENetPacket* packet = enet_packet_create(
        client->buffer, client->nbuffer, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(client->peer, 0, packet);
    enet_host_service(client->host, NULL, 0);
    client->nbuffer = 0;
}

void cl_recv(quark_t* quark, client_t* client, ENetPacket* packet) {
    uint8_t op = packet->data[0];
    switch (op) {
    case SVC_NOTHING: break;
    case SVC_DISCONNECT: {
        uint8_t* reason = packet->data + 1;
        info(quark, "[client] disconnected because '%s'", reason);
        break;
    }
    case SVC_SPAWN_ID: {
        uint8_t id = packet->data[1];
        info(quark, "[client] spawning with id %d", id);
        quark->player = quark->players[id];
        quark->player->is_me = true;
        break;
    }
    }

    enet_packet_destroy(packet);
}

void cl_write_byte(quark_t* quark, client_t* client, uint8_t byte) {
    if (client->nbuffer == CLIENT_MAX_BUFFER) {
        error(quark, "[client] client->nbuffer reached %d", CLIENT_MAX_BUFFER);
        return;
    }

    client->buffer[client->nbuffer++] = byte;
}

void cl_write_bytes(quark_t* quark, client_t* client, uint8_t* bytes, size_t len) {
    for (size_t i = 0; i < len; i++)
        cl_write_byte(quark, client, bytes[i]);
}

bool cl_is_connected(quark_t* quark, client_t* client) {
    if (!client || !client->peer) return false;
    return client->peer->state == ENET_PEER_STATE_CONNECTED;
}

int cl_get_ping(quark_t* quark, client_t* client) {
    if (!client || !client->peer) return -1;
    return client->peer->roundTripTime;
}
