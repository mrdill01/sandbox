#ifndef ITEM_H
#define ITEM_H

#include "render.h"
#include "audio.h"

#include <stdbool.h>
#include <stddef.h>

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOTS INVENTORY_WIDTH * INVENTORY_HEIGHT
#define HOTBAR_SLOTS 4

typedef struct sbox_t sbox_t;

typedef enum {
    ITEM_WEAPON,
} item_type_t;

typedef struct {
    sound_t* fire_sound;
    float fire_rate;
    float last_fire;
    float min_spread;
    float max_spread;
    float recoil;
    float damage;
    
    float is_projectile;
    mesh_t* projectile_mesh;
    material_t* projectile_material;
    float projectile_speed;
} weapon_t;

typedef struct {
    item_type_t type;
    char* name;
    int count;
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    float yaw;
    union {
        weapon_t weapon;
    } data;
} item_t;

typedef struct {
    item_t* items[INVENTORY_SLOTS];
    size_t item_slot;
    bool is_open;
    float last_switch;
    int coins;
} inventory_t;

item_t* item_new(sbox_t* sbox,
    item_type_t type,
    const char* name,
    mesh_t* mesh,
    material_t* materials[MAX_MATERIALS]);
void item_free(sbox_t* sbox, item_t* item);

void inventory_init(sbox_t* sbox, inventory_t* inventory);
void inventory_free(sbox_t* sbox, inventory_t* inventory);
void inventory_give_item(sbox_t* sbox, inventory_t* inventory, item_t* item);
void inventory_select_hotbar_slot(sbox_t* sbox, inventory_t* inventory, int slots);
void inventory_open(sbox_t* sbox, inventory_t* inventory);
void inventory_close(sbox_t* sbox, inventory_t* inventory);
void inventory_toggle(sbox_t* sbox, inventory_t* inventory);

void inventory_set_item(sbox_t* sbox, inventory_t* inventory, int slot, item_t* item);
item_t* inventory_get_item(sbox_t* sbox, inventory_t* inventory);

#endif
