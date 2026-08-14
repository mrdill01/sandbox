#ifndef ITEM_H
#define ITEM_H

#include "render.h"
#include "audio.h"

#include <stdbool.h>
#include <stddef.h>

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 6
#define INVENTORY_SLOTS INVENTORY_WIDTH * INVENTORY_HEIGHT
#define HOTBAR_SLOTS INVENTORY_HEIGHT
#define WEAPON_SWITCH_DELAY 0.5f

typedef struct quark_t quark_t;
typedef struct player_t player_t;

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
    int bullets;
    int mag_size;
    int ammo_loaded;
    int ammo_unloaded;
    float reload_time;
    bool is_reloading;
    float reload_start;
    
    float is_projectile;
    mesh_t* projectile_mesh;
    material_t* projectile_material;
    float projectile_speed;
    bool projectile_gravity;
    bool projectile_particles;
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

item_t* item_new(quark_t* quark,
    item_type_t type,
    const char* name,
    mesh_t* mesh,
    material_t* materials[MAX_MATERIALS]);
void item_free(quark_t* quark, item_t* item);

void weapon_fire(quark_t* quark, weapon_t* weapon, player_t* player);
void weapon_reload(quark_t* quark, weapon_t* weapon, player_t* player);
void weapon_finish_reload(quark_t* quark, weapon_t* weapon, player_t* player);

void inventory_init(quark_t* quark, inventory_t* inventory);
void inventory_free(quark_t* quark, inventory_t* inventory);
void inventory_give_item(quark_t* quark, inventory_t* inventory, item_t* item);
void inventory_select_hotbar_slot(quark_t* quark, inventory_t* inventory, int slots);
void inventory_open(quark_t* quark, inventory_t* inventory);
void inventory_close(quark_t* quark, inventory_t* inventory);
void inventory_toggle(quark_t* quark, inventory_t* inventory);

void inventory_set_item(quark_t* quark, inventory_t* inventory, int slot, item_t* item);
item_t* inventory_get_item(quark_t* quark, inventory_t* inventory);

#endif
