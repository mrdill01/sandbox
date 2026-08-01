#ifndef NET_H
#define NET_H

#define NET_PORT 25565
#define NET_CHANNELS 1
#define NET_TIMEOUT_MSEC 2

typedef struct sbox_t sbox_t;

void net_init(sbox_t* sbox);
void net_free(sbox_t* sbox);

#endif
