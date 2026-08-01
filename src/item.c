#include "item.h"
#include "sbox.h"

item_t* item_new(sbox_t* sbox,
    item_type_t type,
    const char* name,
    mesh_t* mesh,
    material_t* materials[MAX_MATERIALS])
{
    item_t* item = malloc(sizeof(item_t));
    item->type = type;

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
    info(sbox, "loading inventory...");

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

    item_t* shooter = item_new(sbox, ITEM_WEAPON, "Shooter", shooter_mesh, materials);
    inventory_give_item(sbox, inventory, shooter);
    weapon_t* weapon = &shooter->data.weapon;
    weapon->fire_sound = sound_load(sbox, &sbox->audio, "res/sounds/weapons/shooter_fire.wav");
    weapon->fire_rate = 0.1f;
    weapon->last_fire = 0.0f;
    weapon->spread = 0.1f;
    weapon->recoil = 0.15f;
    weapon->damage = 25.0f;
    weapon->is_projectile = false;
    weapon->projectile_mesh = NULL;
    weapon->projectile_material = NULL;
    weapon->projectile_speed = 0.0f;

    mesh_t* pistol_mesh = mesh_load(sbox, "res/meshes/items/pistol.obj");
    item_t* pistol = item_new(sbox, ITEM_WEAPON, "Pistol", pistol_mesh, materials);
    inventory_give_item(sbox, inventory, pistol);
    weapon = &pistol->data.weapon;
    weapon->fire_sound = sound_load(sbox, &sbox->audio, "res/sounds/weapons/pistol_fire.wav");
    weapon->fire_rate = 0.25f;
    weapon->last_fire = 0.0f;
    weapon->spread = 0.08f;
    weapon->recoil = 0.12f;
    weapon->damage = 65.0f;
    weapon->is_projectile = false;
    weapon->projectile_mesh = NULL;
    weapon->projectile_material = NULL;
    weapon->projectile_speed = 0.0f;

    mesh_t* rocket_launcher_mesh = mesh_load(sbox, "res/meshes/items/rocket_launcher.obj");
    item_t* rocket_launcher = item_new(sbox, ITEM_WEAPON, "Rocket Launcher",
        rocket_launcher_mesh, materials);
    inventory_give_item(sbox, inventory, rocket_launcher);
    weapon = &rocket_launcher->data.weapon;
    weapon->fire_sound = sound_load(sbox, &sbox->audio,
        "res/sounds/weapons/rocket_launcher_fire.wav");
    weapon->fire_rate = 0.65f;
    weapon->last_fire = 0.0f;
    weapon->spread = 0.08f;
    weapon->recoil = 0.9f;
    weapon->damage = 140.0f;
    weapon->is_projectile = true;
    weapon->projectile_mesh = mesh_load(sbox, "res/meshes/items/rocket.obj");
    weapon->projectile_material = sbox->renderer.default_material;
    weapon->projectile_speed = 20.0f;

    info(sbox, "inventory loaded.");
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
    if (inventory->item_slot == slot) return;
    
    inventory->item_slot = slot;
    inventory->last_switch = sbox->time;
    a_play(sbox, &sbox->audio, sbox->audio.hotbar_select_sound, GLM_VEC3_ZERO, random(0.9f, 1.1f));
}

void inventory_open(sbox_t* sbox, inventory_t* inventory) {
    if (inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_open_sound, GLM_VEC3_ZERO, random(0.9f, 1.1f));
    inventory->is_open = true;
}

void inventory_close(sbox_t* sbox, inventory_t* inventory) {
    if (!inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_close_sound, GLM_VEC3_ZERO, random(0.9f, 1.1f));
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
