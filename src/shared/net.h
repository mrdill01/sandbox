#ifndef NET_H
#define NET_H

#define NET_PORT 25565
#define NET_CHANNELS 1
#define NET_TIMEOUT_MSEC 0

#define NET_MAX_PLAYERS 32
#define NET_MAX_PLAYER_NAME_LEN 32

typedef struct quark_t quark_t;

typedef enum {
    CSV_NOTHING = 1,
    CSV_SET_NAME,       /* [string name] */
} client_to_server;

typedef enum {
    SVC_NOTHING = 1,
    SVC_DISCONNECT,     /* [string reason] */
    SVC_SPAWN_ID,       /* [u8 id] */
} server_to_client;

void net_init(quark_t* quark);
void net_free(quark_t* quark);

#endif
