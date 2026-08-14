#ifndef MAP_H
#define MAP_H

#include "entity.h"
#include "render.h"

typedef struct quark_t quark_t;

typedef struct {
    bool is_loaded;
    entlist_t entlist;
    texture_t* skybox;
    int coins;
} map_t;

void map_init(quark_t* quark, map_t* map);
void map_load(quark_t* quark, map_t* map);
void map_free(quark_t* quark, map_t* map);
void map_tick(quark_t* quark, map_t* map);

#endif
