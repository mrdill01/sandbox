#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include "../include/enet.h"

typedef struct sbox_t sbox_t;

typedef struct {
    bool is_connected;
    ENetHost* host;
    ENetPeer* peer;
} client_t;

void cl_init(sbox_t* sbox, client_t* client);
void cl_connect(sbox_t* sbox, client_t* client, const char* ip, int port);
void cl_disconnect(sbox_t* sbox, client_t* client);
void cl_tick(sbox_t* sbox, client_t* client);
int cl_get_ping(sbox_t* sbox, client_t* client);

#endif
