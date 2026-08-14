#include "client.h"
#include "quark.h"
#include "net.h"

void cl_init(quark_t* quark, client_t* client) {
    info(quark, "cl_init()...");
    client->host = NULL;
    client->peer = NULL;
    client->out_buffer = NULL;
    client->out_buffer_len = 0;
    client->out_buffer_cap = 0;
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

    int len = strlen(cl_name.string);
    if (len > NET_MAX_PLAYER_NAME) {
        len = NET_MAX_PLAYER_NAME;
        error(quark, "name too long (max %d characters)", NET_MAX_PLAYER_NAME);
    }

    cl_write_byte(quark, client, CL_CMD_SET_NAME);
    cl_write_byte(quark, client, len);
    cl_write_bytes(quark, client, (uint8_t*)cl_name.string, len);

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
    
    if (client->out_buffer_len > 0) {
        ENetPacket* packet = enet_packet_create(
            client->out_buffer,
            client->out_buffer_len,
            ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(client->peer, 0, packet);
        client->out_buffer_len = 0;
    }

    ENetEvent event;
    while (enet_host_service(client->host, &event, NET_TIMEOUT_MSEC) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: break;
        case ENET_EVENT_TYPE_RECEIVE: {
            info(quark, "[client] recv: '%s'", event.packet->data);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        case ENET_EVENT_TYPE_NONE: break;
        }
    }
}

void cl_write_byte(quark_t* quark, client_t* client, uint8_t byte) {
    if (client->out_buffer_len + 1 > client->out_buffer_cap) {
        if (client->out_buffer_cap == 0)
            client->out_buffer_cap = 8;
        else
            client->out_buffer_cap *= 2;
        client->out_buffer = realloc(client->out_buffer, client->out_buffer_cap);
    }

    client->out_buffer[client->out_buffer_len++] = byte;
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
