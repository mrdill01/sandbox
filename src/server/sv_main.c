#include "../server/server.h"
#include "../client/quark.h"
#include "../shared/net.h"
#include "../client/mathlib.h"

void sv_init(quark_t* quark, server_t* server) {
    server->is_running = false;
    server->host = NULL;
    for (size_t i = 0; i < NET_MAX_PLAYERS; i++)
        server->clients[i] = NULL;
}

void sv_start(quark_t* quark, server_t* server, int port) {
    if (server->is_running) return;

    info(quark, "sv_start()...");

    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = port;

    server->host = enet_host_create(&address, NET_MAX_PLAYERS, NET_CHANNELS, 0, 0);
    if (!server->host) {
        error(quark, "failed to setup server (enet_host_create failed)");
        return;
    }

    server->is_running = true;
    info(quark, "server started on port %d!", address.port);
}

void sv_stop(quark_t* quark, server_t* server) {
    if (!server->is_running) return;

    info(quark, "sv_stop()...");

    enet_host_destroy(server->host);
    server->host = NULL;
    server->is_running = false;
    for (size_t i = 0; i < NET_MAX_PLAYERS; i++) {
        sv_client_t* client = server->clients[i];
        if (client) free(client);
    }

    info(quark, "server stopped!");
}

void sv_tick(quark_t* quark, server_t* server) {
    if (!server->is_running) return;

    sv_send(quark, server);

    ENetEvent event;
    while (enet_host_service(server->host, &event, NET_TIMEOUT_MSEC) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_NONE: break;
        case ENET_EVENT_TYPE_CONNECT: {
            char ip[32];
            enet_address_get_host_ip(&event.peer->address, ip, 32);
            info(quark, "[server] client connected from %s", ip);

            sv_create_client(quark, server, event.peer);
            //sv_disconnect_client(quark, server, event.peer, "no reason");
            
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            info(quark, "[server] client disconnected");
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            sv_client_t* client = sv_get_client(quark, server, event.peer);
            sv_recv(quark, server, client, event.packet);
            break;
        }
        }
    }
}

void sv_send(quark_t* quark, server_t* server) {
    for (size_t i = 0; i < NET_MAX_PLAYERS; i++) {
        sv_client_t* client = server->clients[i];
        if (!client) continue;

        sv_write_byte(quark, server, client, SVC_NOTHING);
    }
}

void sv_recv(quark_t* quark, server_t* server, sv_client_t* client, ENetPacket* packet) {
    uint8_t op = packet->data[0] - '0';
    
    switch (op) {
    case CSV_NOTHING: break;
    case CSV_SET_NAME: {
        uint8_t* name = packet->data + 1;
        char ip[32];
        enet_address_get_host_ip(&client->peer->address, ip, 32);

        info(quark, "[server] %s set name to '%s'", ip, name);
        break;
    }
    }

    enet_packet_destroy(packet);
}

void sv_write_byte(quark_t* quark, server_t* server, sv_client_t* client, uint8_t byte) {
    /*if (!client->peer || !server->host) return;
    
    ENetPacket* packet = enet_packet_create(&byte, sizeof(uint8_t), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(client->peer, 0, packet);
    enet_host_service(server->host, NULL, 0);*/
}

void sv_write_bytes(
    quark_t* quark,
    server_t* server,
    sv_client_t* client,
    uint8_t* bytes,
    size_t len)
{
    for (size_t i = 0; i < len; i++)
        sv_write_byte(quark, server, client, bytes[i]);
}

int sv_create_client(quark_t* quark, server_t* server, ENetPeer* peer) {
    int slot = -1;
    for (size_t i = 0; i < NET_MAX_PLAYERS; i++) {
        sv_client_t* client = server->clients[i];
        if (!client) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        char reason[64];
        sprintf(reason, "the server is full (%d/%d players)", NET_MAX_PLAYERS, NET_MAX_PLAYERS);
        sv_disconnect_client(quark, server, peer, reason);
        return -1;
    }

    sv_client_t* client = malloc(sizeof(sv_client_t));
    client->peer = peer;
    client->id = 0;
    client->name = NULL;
    server->clients[slot] = client;
    return slot;
}

void sv_disconnect_client(quark_t* quark, server_t* server, ENetPeer* peer, const char* reason) {
    uint8_t msg[64];
    sprintf((char*)msg, "%d%s", SVC_DISCONNECT, reason);

    ENetPacket* packet = enet_packet_create(msg, sizeof(msg), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);

    enet_peer_disconnect_later(peer, 0);
    enet_host_service(server->host, NULL, 0);
}

sv_client_t* sv_get_client(quark_t* quark, server_t* server, ENetPeer* peer) {
    for (size_t i = 0; i < NET_MAX_PLAYERS; i++) {
        sv_client_t* client = server->clients[i];
        if (!client) continue;
        if (client->peer == peer)
            return client;
    }

    return NULL;
}
