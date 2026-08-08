#ifndef MAP_H
#define MAP_H

#include "entity.h"
#include "render.h"

typedef struct sbox_t sbox_t;

typedef struct {
    bool is_loaded;
    entlist_t entlist;
    texture_t* skybox;
    int coins;
} map_t;

void map_init(sbox_t* sbox, map_t* map);
void map_load(sbox_t* sbox, map_t* map);
void map_free(sbox_t* sbox, map_t* map);
void map_tick(sbox_t* sbox, map_t* map);

#endif
