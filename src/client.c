#include "client.h"
#include "sbox.h"
#include "net.h"

void cl_init(sbox_t* sbox, client_t* client) {
    info(sbox, "cl_init()...");
    client->is_connected = false;
    client->host = NULL;
    client->peer = NULL;
    info(sbox, "client initialized!");
}

void cl_connect(sbox_t* sbox, client_t* client, const char* ip, int port) {
    if (client->is_connected) {
        cl_disconnect(sbox, client);
    }

    client->host = enet_host_create(NULL, 1, NET_CHANNELS, 0, 0);
    if (!client->host) {
        error(sbox, "[client] failed to create host (enet_host_create failed)");
        return;
    }

    ENetAddress address = {0};
    enet_address_set_host(&address, ip);
    address.port = NET_PORT;

    info(sbox, "[client] connecting to %s:%d...", ip, port);

    client->peer = enet_host_connect(client->host, &address, NET_CHANNELS, 0);
    if (!client->peer) {
        error(sbox, "[client] failed to connect! (enet_host_connect failed)");
        return;
    }

    ENetPacket* packet = enet_packet_create("hello", strlen("hello"), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(client->peer, 0, packet);

    client->is_connected = true;
    info(sbox, "[client] connected!");
}

void cl_disconnect(sbox_t* sbox, client_t* client) {
    if (!client->is_connected) return;

    info(sbox, "[client] disconnecting...");
    enet_peer_reset(client->peer);
    enet_host_destroy(client->host);
    client->is_connected = false;
    info(sbox, "[client] disconnected!");
}

void cl_tick(sbox_t* sbox, client_t* client) {
    if (!client->is_connected) return;
    
    ENetEvent event;
    while (enet_host_service(client->host, &event, NET_TIMEOUT_MSEC) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: break;
        case ENET_EVENT_TYPE_RECEIVE: {
            info(sbox, "[client] recv: '%s'", event.packet->data);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        case ENET_EVENT_TYPE_NONE: break;
        }
    }
}

int cl_get_ping(sbox_t* sbox, client_t* client) {
    return client->peer->roundTripTime;
}
