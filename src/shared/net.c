#include "../shared/net.h"
#include "../shared/quark.h"

#define ENET_IMPLEMENTATION
#define ENET_DLL
#include "../../include/enet.h"

void net_init(quark_t* quark) {
    info(quark, "net_init()...");

    int result = enet_initialize();
    if (result < 0) {
        error(quark, "failed to initialize enet (%d)", result);
        return;
    }

    info(quark, "network initialized!");
}

void net_free(quark_t* quark) {
    enet_deinitialize();
}
