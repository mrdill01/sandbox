#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/enet.h"

typedef struct quark_t quark_t;

typedef struct {
    ENetHost* host;
    ENetPeer* peer;
    uint8_t* out_buffer;
    size_t out_buffer_len;
    size_t out_buffer_cap;
} client_t;

void cl_init(quark_t* quark, client_t* client);
void cl_connect(quark_t* quark, client_t* client, const char* ip, int port);
void cl_disconnect(quark_t* quark, client_t* client);
void cl_tick(quark_t* quark, client_t* client);

void cl_write_byte(quark_t* quark, client_t* client, uint8_t byte);
void cl_write_bytes(quark_t* quark, client_t* client, uint8_t* bytes, size_t len);

bool cl_is_connected(quark_t* quark, client_t* client);
int cl_get_ping(quark_t* quark, client_t* client);

#endif
