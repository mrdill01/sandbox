#include "item.h"
#include "sbox.h"

void inventory_init(sbox_t* sbox, inventory_t* inventory) {
    for (int i = 0; i < INVENTORY_SLOTS; i++)
        inventory->items[i].is_free = true;
    inventory->item_slot = 0;
    inventory->is_open = false;
    inventory->last_switch = 0.0f;
}

void inventory_select_hotbar_slot(sbox_t* sbox, inventory_t* inventory, int slot) {
    inventory->item_slot = slot;
    inventory->last_switch = sbox->time;
    a_play(sbox, &sbox->audio, sbox->audio.hotbar_select_sound, random(0.9f, 1.1f));
}

void inventory_open(sbox_t* sbox, inventory_t* inventory) {
    if (inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_open_sound, random(0.9f, 1.1f));
    inventory->is_open = true;
}

void inventory_close(sbox_t* sbox, inventory_t* inventory) {
    if (!inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_close_sound, random(0.9f, 1.1f));
    inventory->is_open = false;
}

void inventory_toggle(sbox_t* sbox, inventory_t* inventory) {
    if (inventory->is_open) inventory_close(sbox, inventory);
    else inventory_open(sbox, inventory);
}

void inventory_set_item(sbox_t* sbox, inventory_t* inventory, int slot, item_t item) {
    inventory->items[slot] = item;
}
