#ifndef NET_H
#define NET_H

#define NET_PORT 25565
#define NET_CHANNELS 1
#define NET_TIMEOUT_MSEC 0
#define NET_MAX_PLAYER_NAME 32

typedef struct sbox_t sbox_t;

typedef enum {
    CL_CMD_SET_NAME,
} client_cmd_t;

void net_init(sbox_t* sbox);
void net_free(sbox_t* sbox);

#endif
