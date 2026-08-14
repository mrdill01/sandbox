#ifndef NET_H
#define NET_H

#define NET_PORT 25565
#define NET_CHANNELS 1
#define NET_TIMEOUT_MSEC 0
#define NET_MAX_PLAYER_NAME 32

typedef struct quark_t quark_t;

typedef enum {
    CL_CMD_SET_NAME,
} client_cmd_t;

void net_init(quark_t* quark);
void net_free(quark_t* quark);

#endif
