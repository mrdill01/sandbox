#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "../include/enet.h"

typedef struct sbox_t sbox_t;

typedef struct {
    bool is_running;
    ENetHost* host;
} server_t;

void sv_init(sbox_t* sbox, server_t* server);
void sv_start(sbox_t* sbox, server_t* server, int port);
void sv_stop(sbox_t* sbox, server_t* server);
void sv_tick(sbox_t* sbox, server_t* server);

#endif
