#ifndef SERVER_H
#define SERVER_H

#include "../shared/net.h"
#include "../shared/player.h"

#include <stdbool.h>

#include "../../include/enet.h"

#define SERVER_MAX_BUFFER 1024

typedef struct quark_t quark_t;

typedef struct {
    bool is_running;
    ENetHost* host;
    struct sv_client_t* clients[NET_MAX_PLAYERS];
} server_t;

typedef struct sv_client_t {
    ENetPeer* peer;
    int id;
    char* name;

    uint8_t buffer[SERVER_MAX_BUFFER];
    size_t nbuffer;
} sv_client_t;

void sv_init(quark_t* quark, server_t* server);
void sv_start(quark_t* quark, server_t* server, int port);
void sv_stop(quark_t* quark, server_t* server);

void sv_tick(quark_t* quark, server_t* server);
void sv_send(quark_t* quark, server_t* server);
void sv_recv(quark_t* quark, server_t* server, sv_client_t* client, ENetPacket* packet);

void sv_write_byte(quark_t* quark, server_t* server, sv_client_t* client, uint8_t byte);
void sv_write_bytes(
    quark_t* quark,
    server_t* server,
    sv_client_t* client,
    uint8_t* bytes,
    size_t len);

int sv_create_client(quark_t* quark, server_t* server, ENetPeer* peer);
void sv_disconnect_client(quark_t* quark, server_t* server, ENetPeer* peer, const char* reason);
sv_client_t* sv_get_client(quark_t* quark, server_t* server, ENetPeer* peer);

#endif
