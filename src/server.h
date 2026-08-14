#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "../include/enet.h"

typedef struct quark_t quark_t;

typedef struct {
    bool is_running;
    ENetHost* host;
} server_t;

void sv_init(quark_t* quark, server_t* server);
void sv_start(quark_t* quark, server_t* server, int port);
void sv_stop(quark_t* quark, server_t* server);
void sv_tick(quark_t* quark, server_t* server);

#endif
