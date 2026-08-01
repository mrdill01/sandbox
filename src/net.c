#include "net.h"
#include "sbox.h"

#define ENET_IMPLEMENTATION
#define ENET_DLL
#include "../include/enet.h"

void net_init(sbox_t* sbox) {
    info(sbox, "net_init()...");

    int result = enet_initialize();
    if (result < 0) {
        error(sbox, "failed to initialize enet (%d)", result);
        return;
    }

    info(sbox, "network initialized!");
}

void net_free(sbox_t* sbox) {
    enet_deinitialize();
}
