#include "item.h"
#include "quark.h"

item_t* item_new(quark_t* quark,
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

void item_free(quark_t* quark, item_t* item) {
    if (!item) return;
    free(item->name);
    free(item);
}

void weapon_fire(quark_t* quark, weapon_t* weapon, player_t* player) {
    if (quark->time - weapon->last_fire < weapon->fire_rate)
        return;

    if (quark->time - player->inventory.last_switch < WEAPON_SWITCH_DELAY)
        return;
    
    if (weapon->is_reloading)
        return;

    if (weapon->ammo_loaded == 0 && weapon->ammo_unloaded == 0) {
        a_play(quark,
            &quark->audio, quark->audio.gun_click_sound, player->position, random(0.85f, 1.15f));
        return;
    }
    
    weapon->ammo_loaded--;

    vec3 start;
    glm_vec3_copy(quark->renderer.camera.position, start);
    
    if (player->is_thirdperson) {
        vec3 forward;
        glm_vec3_copy(quark->renderer.camera.forward, forward);
        glm_vec3_scale(forward, PLAYER_THIRDPERSON_CAMERA_LENGTH, forward);
        glm_vec3_add(start, forward, start);
    }
    
    weapon->last_fire = quark->time;
    a_play(quark, &quark->audio, weapon->fire_sound, player->position, 1.0f);

    player->item_anim[2] -= (player->buttons & PLAYER_BUTTON_AIM) ?
        weapon->recoil * 0.04f : weapon->recoil;
    
    quark->renderer.camera.shake[0] += weapon->recoil * 14.0f;
    quark->renderer.camera.shake[1] += weapon->recoil * 14.0f * random(-1.0f, 1.0f);
    quark->renderer.camera.shake[2] += weapon->recoil * 14.0f * random(-1.0f, 1.0f);

    for (int i = 0; i < weapon->bullets; i++) {
        camera_t* camera = &quark->renderer.camera;
        vec3 direction;
        glm_vec3_copy(camera->forward, direction);

        float spread = lerp(
            weapon->min_spread,
            weapon->max_spread,
            1.0f - player_get_accuracy(quark, player));
        
        direction[0] += random(-spread, spread);
        direction[1] += random(-spread, spread);
        direction[2] += random(-spread, spread);
        glm_normalize(direction);

        if (weapon->is_projectile) {
            vec3 velocity;
            glm_vec3_copy(direction, velocity);
            glm_vec3_scale(velocity, weapon->projectile_speed, velocity);

            entity_t* projectile = NULL;
            entity_init_projectile(quark,
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
            entlist_add(quark, &quark->map.entlist, projectile);

        } else {
            float max_distance = 50.0f;
            trace_result_t trace;

            bool hit = phys_line_trace(quark, start, direction, max_distance,
                &quark->map.entlist, player->id, NULL, 0, &trace);
            if (hit) {
                sound_t* bullet_hit_sound = quark->audio.bullet_hit_sounds[trace.phys_mat];
                a_play(quark, &quark->audio, bullet_hit_sound, trace.point, random(0.8f, 1.2f));
                r_add_partfx_shoot_hit(quark, &quark->renderer, trace);

                if (trace.water_level > 0.0f) {
                    r_add_partfx_shoot_hit_water(quark, &quark->renderer, trace);
                }

                if (trace.player_id != -1) {
                    player_add_damage(quark, quark->players[trace.player_id], weapon->damage);
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

                r_add_line(quark, &quark->renderer, start, end, COLOR_LIGHT_BLUE, 2.5f);
            }

            vec3 beam_start;
            glm_vec3_copy(start, beam_start);
            
            vec3 forward;
            glm_vec3_copy(camera->forward, forward);
            glm_vec3_scale(forward, 0.25f, forward);

            glm_vec3_add(beam_start, forward, beam_start);

            r_add_partfx_shoot_beam(quark, &quark->renderer, beam_start, direction,
                (hit) ? trace.distance : max_distance);
        }
    }

    if (weapon->ammo_loaded == 0) {
        weapon_reload(quark, weapon, player);
    }
}

void weapon_reload(quark_t* quark, weapon_t* weapon, player_t* player) {
    if (weapon->is_reloading || weapon->ammo_unloaded == 0) return;
    weapon->is_reloading = true;
    weapon->reload_start = quark->time;
}

void weapon_finish_reload(quark_t* quark, weapon_t* weapon, player_t* player) {
    if (!weapon->is_reloading) return;

    weapon->is_reloading = false;
    int difference = weapon->mag_size - weapon->ammo_loaded;
    weapon->ammo_loaded = min(weapon->mag_size, weapon->ammo_unloaded);
    weapon->ammo_unloaded -= difference;
    weapon->ammo_unloaded = max(weapon->ammo_unloaded, 0);
}

void inventory_init(quark_t* quark, inventory_t* inventory) {
    info(quark, "loading inventory...");

    for (int i = 0; i < INVENTORY_SLOTS; i++)
        inventory->items[i] = NULL;
    inventory->item_slot = 0;
    inventory->is_open = false;
    inventory->last_switch = 0.0f;
    inventory->coins = 0;

    mesh_t* shooter_mesh = mesh_load(quark, "res/meshes/items/shooter.obj");
    material_t* test = material_load(quark,
        "test",
        "res/textures/materials/test.png",
        "res/textures/materials/test_r.png",
        "res/textures/materials/test_n.png",
        1, 1, false, PHYS_MAT_METAL);
    material_t* materials[MAX_MATERIALS] = {test};

    item_t* shooter = item_new(quark, ITEM_WEAPON, "Shooter", shooter_mesh, materials);
    inventory_give_item(quark, inventory, shooter);
    weapon_t* weapon = &shooter->data.weapon;
    weapon->fire_sound = sound_load(quark, &quark->audio, "res/sounds/weapons/shooter_fire.wav");
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

    mesh_t* pistol_mesh = mesh_load(quark, "res/meshes/items/pistol.obj");
    item_t* pistol = item_new(quark, ITEM_WEAPON, "Pistol", pistol_mesh, materials);
    inventory_give_item(quark, inventory, pistol);
    weapon = &pistol->data.weapon;
    weapon->fire_sound = sound_load(quark, &quark->audio, "res/sounds/weapons/pistol_fire.wav");
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

    mesh_t* rocket_launcher_mesh = mesh_load(quark, "res/meshes/items/rocket_launcher.obj");
    item_t* rocket_launcher = item_new(quark, ITEM_WEAPON, "Rocket Launcher",
        rocket_launcher_mesh, materials);
    inventory_give_item(quark, inventory, rocket_launcher);
    weapon = &rocket_launcher->data.weapon;
    weapon->fire_sound = sound_load(quark, &quark->audio,
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
    weapon->reload_time = 2.4f;
    weapon->is_reloading = false;
    weapon->reload_start = 0.0f;
    weapon->is_projectile = true;
    weapon->projectile_mesh = mesh_load(quark, "res/meshes/items/rocket.obj");
    weapon->projectile_material = quark->renderer.default_material;
    weapon->projectile_speed = 16.0f;
    weapon->projectile_gravity = false;
    weapon->projectile_particles = true;

    mesh_t* sniper_mesh = mesh_load(quark, "res/meshes/items/sniper.obj");
    item_t* sniper = item_new(quark, ITEM_WEAPON, "Sniper", sniper_mesh, materials);
    inventory_give_item(quark, inventory, sniper);
    weapon = &sniper->data.weapon;
    weapon->fire_sound = sound_load(quark, &quark->audio, "res/sounds/weapons/sniper_fire.wav");
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

    mesh_t* shotgun_mesh = mesh_load(quark, "res/meshes/items/shotgun.obj");
    item_t* shotgun = item_new(quark, ITEM_WEAPON, "Shotgun", shotgun_mesh, materials);
    inventory_give_item(quark, inventory, shotgun);
    weapon = &shotgun->data.weapon;
    weapon->fire_sound = sound_load(quark, &quark->audio, "res/sounds/weapons/shotgun_fire.wav");
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

    mesh_t* landmine_mesh = mesh_load(quark, "res/meshes/items/landmine.obj");
    item_t* landmine = item_new(quark, ITEM_WEAPON, "Landmine",
        landmine_mesh, materials);
    inventory_give_item(quark, inventory, landmine);
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
    weapon->projectile_mesh = mesh_load(quark, "res/meshes/items/landmine.obj");
    weapon->projectile_material = quark->renderer.default_material;
    weapon->projectile_speed = 0.0f;
    weapon->projectile_gravity = true;
    weapon->projectile_particles = false;

    info(quark, "inventory loaded.");
}

void inventory_free(quark_t* quark, inventory_t* inventory) {
    for (size_t i = 0; i < INVENTORY_SLOTS; i++) {
        item_free(quark, inventory->items[i]);
    }
}

void inventory_give_item(quark_t* quark, inventory_t* inventory, item_t* item) {
    for (size_t i = 0; i < INVENTORY_SLOTS; i++) {
        if (!inventory->items[i]) {
            inventory->items[i] = item;
            break;
        }
    }
}

void inventory_select_hotbar_slot(quark_t* quark, inventory_t* inventory, int slot) {
    if (inventory->item_slot == slot) return;
    
    inventory->item_slot = slot;
    inventory->last_switch = quark->time;

    sound_t* sound = quark->audio.hotbar_select_sounds[(int)random(0, NUM_HOTBAR_SELECT_SOUNDS)];
    a_play(quark, &quark->audio, sound, quark->renderer.camera.position, random(0.9f, 1.1f));
}

void inventory_open(quark_t* quark, inventory_t* inventory) {
    if (inventory->is_open) return;
    a_play(quark, &quark->audio, quark->audio.inventory_open_sound,
        quark->renderer.camera.position, random(0.9f, 1.1f));
    inventory->is_open = true;
}

void inventory_close(quark_t* quark, inventory_t* inventory) {
    if (!inventory->is_open) return;
    a_play(quark, &quark->audio, quark->audio.inventory_close_sound,
        quark->renderer.camera.position, random(0.9f, 1.1f));
    inventory->is_open = false;
}

void inventory_toggle(quark_t* quark, inventory_t* inventory) {
    if (inventory->is_open) inventory_close(quark, inventory);
    else inventory_open(quark, inventory);
}

void inventory_set_item(quark_t* quark, inventory_t* inventory, int slot, item_t* item) {
    inventory->items[slot] = item;
}

item_t* inventory_get_item(quark_t* quark, inventory_t* inventory) {
    return inventory->items[inventory->item_slot];
}
