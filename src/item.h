#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOTS INVENTORY_WIDTH * INVENTORY_HEIGHT
#define HOTBAR_SLOTS 4

typedef struct sbox_t sbox_t;

typedef struct {
    bool is_free;
    const char* name;
    int count;
} item_t;

typedef struct {
    item_t items[INVENTORY_SLOTS];
    size_t item_slot;
    bool is_open;
    float last_switch;
} inventory_t;

void inventory_init(sbox_t* sbox, inventory_t* inventory);
void inventory_select_hotbar_slot(sbox_t* sbox, inventory_t* inventory, int slots);
void inventory_open(sbox_t* sbox, inventory_t* inventory);
void inventory_close(sbox_t* sbox, inventory_t* inventory);
void inventory_toggle(sbox_t* sbox, inventory_t* inventory);

void inventory_set_item(sbox_t* sbox, inventory_t* inventory, int slot, item_t item);

#endif
