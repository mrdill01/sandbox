#include "item.h"
#include "sbox.h"

item_t* item_new(sbox_t* sbox,
    const char* name,
    mesh_t* mesh,
    material_t* materials[MAX_MATERIALS])
{
    item_t* item = malloc(sizeof(item_t));
    
    size_t len = strlen(name);
    item->name = malloc(len + 1);
    strcpy(item->name, name);
    item->name[len] = '\0';
    
    item->count = 0;
    item->mesh = mesh;
    memcpy(item->materials, materials, sizeof(material_t*) * MAX_MATERIALS);
    return item;
}

void item_free(sbox_t* sbox, item_t* item) {
    if (!item) return;
    free(item->name);
    free(item);
}

void inventory_init(sbox_t* sbox, inventory_t* inventory) {
    for (int i = 0; i < INVENTORY_SLOTS; i++)
        inventory->items[i] = NULL;
    inventory->item_slot = 0;
    inventory->is_open = false;
    inventory->last_switch = 0.0f;

    mesh_t* shooter_mesh = mesh_load(sbox, "res/meshes/items/shooter.obj");
    material_t* test = material_load(sbox,
        "test",
        "res/textures/materials/test.png",
        "res/textures/materials/test_r.png",
        "res/textures/materials/test_n.png",
        1, 1, false, PHYS_MAT_METAL);
    material_t* materials[MAX_MATERIALS] = {test};

    item_t* shooter = item_new(sbox, "shooter", shooter_mesh, materials);
    inventory_give_item(sbox, inventory, shooter);

    mesh_t* pistol_mesh = mesh_load(sbox, "res/meshes/items/pistol.obj");
    item_t* pistol = item_new(sbox, "pistol", pistol_mesh, materials);
    inventory_give_item(sbox, inventory, pistol);
}

void inventory_free(sbox_t* sbox, inventory_t* inventory) {
    for (size_t i = 0; i < INVENTORY_SLOTS; i++) {
        item_free(sbox, inventory->items[i]);
    }
}

void inventory_give_item(sbox_t* sbox, inventory_t* inventory, item_t* item) {
    for (size_t i = 0; i < INVENTORY_SLOTS; i++) {
        if (!inventory->items[i]) {
            inventory->items[i] = item;
            break;
        }
    }
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

void inventory_set_item(sbox_t* sbox, inventory_t* inventory, int slot, item_t* item) {
    inventory->items[slot] = item;
}

item_t* inventory_get_item(sbox_t* sbox, inventory_t* inventory) {
    return inventory->items[inventory->item_slot];
}
