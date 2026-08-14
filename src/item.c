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
    item->yaw = 90.0f;
    return item;
}

void item_free(sbox_t* sbox, item_t* item) {
    if (!item) return;
    free(item->name);
    free(item);
}

void weapon_fire(sbox_t* sbox, weapon_t* weapon, player_t* player) {
    if (sbox->time - weapon->last_fire < weapon->fire_rate)
        return;

    if (sbox->time - player->inventory.last_switch < WEAPON_SWITCH_DELAY)
        return;
    
    if (weapon->is_reloading)
        return;

    if (weapon->ammo_loaded == 0 && weapon->ammo_unloaded == 0) {
        a_play(sbox,
            &sbox->audio, sbox->audio.gun_click_sound, player->position, random(0.85f, 1.15f));
        return;
    }
    
    weapon->ammo_loaded--;

    vec3 start;
    glm_vec3_copy(sbox->renderer.camera.position, start);
    
    if (player->is_thirdperson) {
        vec3 forward;
        glm_vec3_copy(sbox->renderer.camera.forward, forward);
        glm_vec3_scale(forward, PLAYER_THIRDPERSON_CAMERA_LENGTH, forward);
        glm_vec3_add(start, forward, start);
    }
    
    weapon->last_fire = sbox->time;
    a_play(sbox, &sbox->audio, weapon->fire_sound, player->position, 1.0f);

    player->item_anim[2] -= (player->buttons & PLAYER_BUTTON_AIM) ?
        weapon->recoil * 0.04f : weapon->recoil;
    
    sbox->renderer.camera.shake[0] += weapon->recoil * 14.0f;
    sbox->renderer.camera.shake[1] += weapon->recoil * 14.0f * random(-1.0f, 1.0f);
    sbox->renderer.camera.shake[2] += weapon->recoil * 14.0f * random(-1.0f, 1.0f);

    for (int i = 0; i < weapon->bullets; i++) {
        camera_t* camera = &sbox->renderer.camera;
        vec3 direction;
        glm_vec3_copy(camera->forward, direction);

        float spread = lerp(
            weapon->min_spread,
            weapon->max_spread,
            1.0f - player_get_accuracy(sbox, player));
        
        direction[0] += random(-spread, spread);
        direction[1] += random(-spread, spread);
        direction[2] += random(-spread, spread);
        glm_normalize(direction);

        if (weapon->is_projectile) {
            vec3 velocity;
            glm_vec3_copy(direction, velocity);
            glm_vec3_scale(velocity, weapon->projectile_speed, velocity);

            entity_t* projectile = NULL;
            entity_init_projectile(sbox,
                "rocket",
                start,
                player->id,
                weapon->projectile_mesh,
                velocity,
                weapon->projectile_speed,
                weapon->projectile_gravity,
                weapon->damage, 
                weapon->projectile_particles,
                &projectile);
            projectile->data.projectile.materials[0] = weapon->projectile_material;
            glm_quat_copy(camera->rotation, projectile->rotation);
            entlist_add(sbox, &sbox->map.entlist, projectile);

        } else {
            float max_distance = 50.0f;
            trace_result_t trace;

            bool hit = phys_line_trace(sbox, start, direction, max_distance,
                &sbox->map.entlist, player->id, &trace);
            if (hit) {
                sound_t* bullet_hit_sound = sbox->audio.bullet_hit_sounds[trace.phys_mat];
                a_play(sbox, &sbox->audio, bullet_hit_sound, trace.point, random(0.8f, 1.2f));
                r_add_partfx_shoot_hit(sbox, &sbox->renderer, trace);

                if (trace.water_level > 0.0f) {
                    r_add_partfx_shoot_hit_water(sbox, &sbox->renderer, trace);
                }

                if (trace.player_id != -1) {
                    player_add_damage(sbox, sbox->players[trace.player_id], weapon->damage);
                }
            }

            if (r_debug_bullets.value) {
                vec3 end;
                glm_vec3_copy(start, end);

                float distance = max_distance;
                if (hit)
                    distance = trace.distance;
                
                end[0] += direction[0] * distance;
                end[1] += direction[1] * distance;
                end[2] += direction[2] * distance;

                r_add_line(sbox, &sbox->renderer, start, end, COLOR_LIGHT_BLUE, 2.5f);
            }

            vec3 beam_start;
            glm_vec3_copy(start, beam_start);
            
            vec3 forward;
            glm_vec3_copy(camera->forward, forward);
            glm_vec3_scale(forward, 0.25f, forward);

            glm_vec3_add(beam_start, forward, beam_start);

            r_add_partfx_shoot_beam(sbox, &sbox->renderer, beam_start, direction,
                (hit) ? trace.distance : max_distance);
        }
    }

    if (weapon->ammo_loaded == 0) {
        weapon_reload(sbox, weapon, player);
    }
}

void weapon_reload(sbox_t* sbox, weapon_t* weapon, player_t* player) {
    if (weapon->is_reloading || weapon->ammo_unloaded == 0) return;
    weapon->is_reloading = true;
    weapon->reload_start = sbox->time;
}

void weapon_finish_reload(sbox_t* sbox, weapon_t* weapon, player_t* player) {
    if (!weapon->is_reloading) return;

    weapon->is_reloading = false;
    int difference = weapon->mag_size - weapon->ammo_loaded;
    weapon->ammo_loaded = min(weapon->mag_size, weapon->ammo_unloaded);
    weapon->ammo_unloaded -= difference;
    weapon->ammo_unloaded = max(weapon->ammo_unloaded, 0);
}

void inventory_init(sbox_t* sbox, inventory_t* inventory) {
    info(sbox, "loading inventory...");

    for (int i = 0; i < INVENTORY_SLOTS; i++)
        inventory->items[i] = NULL;
    inventory->item_slot = 0;
    inventory->is_open = false;
    inventory->last_switch = 0.0f;
    inventory->coins = 0;

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
    weapon->min_spread = 0.075f;
    weapon->max_spread = 0.2f;
    weapon->recoil = 0.15f;
    weapon->damage = 25.0f;
    weapon->bullets = 1;
    weapon->mag_size = 30;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 3;
    weapon->reload_time = 3.4f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
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
    weapon->min_spread = 0.065f;
    weapon->max_spread = 0.25f;
    weapon->recoil = 0.12f;
    weapon->damage = 65.0f;
    weapon->bullets = 1;
    weapon->mag_size = 12;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 4;
    weapon->reload_time = 2.4f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
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
    weapon->min_spread = 0.01f;
    weapon->max_spread = 0.125f;
    weapon->recoil = 0.5f;
    weapon->damage = 140.0f;
    weapon->bullets = 1;
    weapon->mag_size = 1;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 20;
    weapon->reload_time = 2.7f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
    weapon->is_projectile = true;
    weapon->projectile_mesh = mesh_load(sbox, "res/meshes/items/rocket.obj");
    weapon->projectile_material = sbox->renderer.default_material;
    weapon->projectile_speed = 16.0f;
    weapon->projectile_gravity = false;
    weapon->projectile_particles = true;

    mesh_t* sniper_mesh = mesh_load(sbox, "res/meshes/items/sniper.obj");
    item_t* sniper = item_new(sbox, ITEM_WEAPON, "Sniper", sniper_mesh, materials);
    inventory_give_item(sbox, inventory, sniper);
    weapon = &sniper->data.weapon;
    weapon->fire_sound = sound_load(sbox, &sbox->audio, "res/sounds/weapons/sniper_fire.wav");
    weapon->fire_rate = 1.7f;
    weapon->last_fire = 0.0f;
    weapon->min_spread = 0.01f;
    weapon->max_spread = 0.5f;
    weapon->recoil = 1.0f;
    weapon->damage = 110.0f;
    weapon->bullets = 1;
    weapon->mag_size = 6;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 4;
    weapon->reload_time = 5.5f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
    weapon->is_projectile = false;
    weapon->projectile_mesh = NULL;
    weapon->projectile_material = NULL;
    weapon->projectile_speed = 0.0f;

    mesh_t* shotgun_mesh = mesh_load(sbox, "res/meshes/items/shotgun.obj");
    item_t* shotgun = item_new(sbox, ITEM_WEAPON, "Shotgun", shotgun_mesh, materials);
    inventory_give_item(sbox, inventory, shotgun);
    weapon = &shotgun->data.weapon;
    weapon->fire_sound = sound_load(sbox, &sbox->audio, "res/sounds/weapons/shotgun_fire.wav");
    weapon->fire_rate = 0.5f;
    weapon->last_fire = 0.0f;
    weapon->min_spread = 0.065f;
    weapon->max_spread = 0.5f;
    weapon->recoil = 1.0f;
    weapon->damage = 110.0f;
    weapon->bullets = 7;
    weapon->mag_size = 4;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 5;
    weapon->reload_time = 4.0f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
    weapon->is_projectile = false;
    weapon->projectile_mesh = NULL;
    weapon->projectile_material = NULL;
    weapon->projectile_speed = 0.0f;

    mesh_t* landmine_mesh = mesh_load(sbox, "res/meshes/items/landmine.obj");
    item_t* landmine = item_new(sbox, ITEM_WEAPON, "Landmine",
        landmine_mesh, materials);
    inventory_give_item(sbox, inventory, landmine);
    weapon = &landmine->data.weapon;
    weapon->fire_sound = NULL;
    weapon->fire_rate = 1.5f;
    weapon->last_fire = 0.0f;
    weapon->min_spread = 0.0f;
    weapon->max_spread = 0.0f;
    weapon->recoil = 0.0f;
    weapon->damage = 125.0f;
    weapon->bullets = 1;
    weapon->mag_size = 1;
    weapon->ammo_loaded = weapon->mag_size;
    weapon->ammo_unloaded = weapon->mag_size * 3;
    weapon->reload_time = 5.0f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
    weapon->is_projectile = true;
    weapon->projectile_mesh = mesh_load(sbox, "res/meshes/items/landmine.obj");
    weapon->projectile_material = sbox->renderer.default_material;
    weapon->projectile_speed = 0.0f;
    weapon->projectile_gravity = true;
    weapon->projectile_particles = false;

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

    sound_t* sound = sbox->audio.hotbar_select_sounds[(int)random(0, NUM_HOTBAR_SELECT_SOUNDS)];
    a_play(sbox, &sbox->audio, sound, sbox->renderer.camera.position, random(0.9f, 1.1f));
}

void inventory_open(sbox_t* sbox, inventory_t* inventory) {
    if (inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_open_sound,
        sbox->renderer.camera.position, random(0.9f, 1.1f));
    inventory->is_open = true;
}

void inventory_close(sbox_t* sbox, inventory_t* inventory) {
    if (!inventory->is_open) return;
    a_play(sbox, &sbox->audio, sbox->audio.inventory_close_sound,
        sbox->renderer.camera.position, random(0.9f, 1.1f));
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
