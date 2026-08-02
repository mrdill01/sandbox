#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/enet.h"

typedef struct sbox_t sbox_t;

typedef struct {
    ENetHost* host;
    ENetPeer* peer;
    uint8_t* out_buffer;
    size_t out_buffer_len;
    size_t out_buffer_cap;
} client_t;

void cl_init(sbox_t* sbox, client_t* client);
void cl_connect(sbox_t* sbox, client_t* client, const char* ip, int port);
void cl_disconnect(sbox_t* sbox, client_t* client);
void cl_tick(sbox_t* sbox, client_t* client);

void cl_write_byte(sbox_t* sbox, client_t* client, uint8_t byte);
void cl_write_bytes(sbox_t* sbox, client_t* client, uint8_t* bytes, size_t len);

bool cl_is_connected(sbox_t* sbox, client_t* client);
int cl_get_ping(sbox_t* sbox, client_t* client);

#endif
