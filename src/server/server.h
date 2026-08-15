#ifndef SERVER_H
#define SERVER_H

#include "../shared/net.h"

#include <stdbool.h>

#include "../../include/enet.h"

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
} sv_client_t;

void sv_init(quark_t* quark, server_t* server);
void sv_start(quark_t* quark, server_t* server, int port);
void sv_stop(quark_t* quark, server_t* server);
void sv_tick(quark_t* quark, server_t* server);

int sv_create_client(quark_t* quark, server_t* server, ENetPeer* peer);
void sv_disconnect_client(quark_t* quark, server_t* server, ENetPeer* peer, const char* reason);

#endif
