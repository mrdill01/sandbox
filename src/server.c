#include "server.h"
#include "sbox.h"
#include "net.h"
#include "math.h"

void sv_init(sbox_t* sbox, server_t* server) {
    server->is_running = false;
    server->host = NULL;
}

void sv_start(sbox_t* sbox, server_t* server, int port) {
    if (server->is_running) return;

    info(sbox, "sv_start()...");

    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = port;

    server->host = enet_host_create(&address, MAX_PLAYERS, NET_CHANNELS, 0, 0);
    if (!server->host) {
        error(sbox, "failed to setup server (enet_host_create failed)");
        return;
    }

    server->is_running = true;
    info(sbox, "server started on port %d!", address.port);
}

void sv_stop(sbox_t* sbox, server_t* server) {
    if (!server->is_running) return;

    info(sbox, "sv_stop()...");
    enet_host_destroy(server->host);
    server->host = NULL;
    server->is_running = false;
    info(sbox, "server stopped!");
}

void sv_tick(sbox_t* sbox, server_t* server) {
    if (!server->is_running) return;

    ENetEvent event;
    while (enet_host_service(server->host, &event, NET_TIMEOUT_MSEC) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            char ip[32];
            enet_address_get_host_ip(&event.peer->address, ip, 32);
            info(sbox, "[server] client connected from %s", ip);

            ENetPacket* packet = enet_packet_create("welcome", strlen("welcome"), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, packet);
            
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            info(sbox, "[server] recv: '%s'", event.packet->data);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            info(sbox, "[server] client disconnected");
            break;
        }
        case ENET_EVENT_TYPE_NONE: break;
        }
    }
}
