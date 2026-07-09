#include "map.h"
#include "sbox.h"

void map_load(sbox_t* sbox, map_t* map) {
	entlist_init(sbox, &map->entlist);

    mesh_t* floor_mesh = mesh_load(sbox, "res/meshes/floor.obj");
    mesh_t* wall_mesh = mesh_load(sbox, "res/meshes/wall.obj");
    mesh_t* crate_mesh = mesh_load(sbox, "res/meshes/crate.obj");
    mesh_t* barrel_mesh = mesh_load(sbox, "res/meshes/barrel.obj");
    mesh_t* cactus_mesh = mesh_load(sbox, "res/meshes/cactus.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(sbox, "res/meshes/chainlink_fence.obj");
    mesh_t* car_mesh = mesh_load(sbox, "res/meshes/car.obj");
    mesh_t* tommy_gun_mesh = mesh_load(sbox, "res/meshes/tommy_gun.obj");
    
    material_t* crate = material_load(sbox,
        "res/textures/crate.png",
        "res/textures/crate_r.png",
        "res/textures/crate_n.png",
        1, 1, false);

    material_t* chainlink = material_load(sbox,
        "res/textures/chainlink.png",
        "res/textures/chainlink_r.png",
        "res/textures/chainlink_n.png",
        20, 20, true);

    material_t* metal = material_load(sbox,
        "res/textures/metal.png",
        "res/textures/metal_r.png",
        "res/textures/metal_n.png",
        1, 1, false);

    material_t* barrel = material_load(sbox,
        "res/textures/barrel.png",
        "res/textures/barrel_r.png",
        "res/textures/barrel_n.png",
        1, 1, false);

    material_t* barrel_top = material_load(sbox,
        "res/textures/barrel_top.png",
        "res/textures/barrel_top_r.png",
        "res/textures/barrel_top_n.png",
        1, 1, false);

    material_t* wood = material_load(sbox,
        "res/textures/wood.png",
        "res/textures/wood_r.png",
        "res/textures/wood_n.png",
        1, 1, false);

    material_t* brick = material_load(sbox,
        "res/textures/brick.png",
        "res/textures/brick_r.png",
        "res/textures/brick_n.png",
        8, 8, false);

    material_t* cactus = material_load(sbox,
        "res/textures/cactus.png",
        "res/textures/cactus_r.png",
        "res/textures/cactus_n.png",
        3, 3, false);

    entity_t* entity;
    entity_init(sbox, "floor", 0.0f, -0.5f, 0.0f, &entity);
    entity->mesh = floor_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "floor(2)", 8.0f, -0.5f, 0.0f, &entity);
    entity->mesh = floor_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "floor(3)", 8.0f, -0.5f, 8.0f, &entity);
    entity->mesh = floor_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "sphere", 1.5f, 1.5f, -1.5f, &entity);
    entity->mesh = sbox->renderer.sphere_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "wall", 0.0f, -0.5f, 4.0f, &entity);
    entity->mesh = wall_mesh;
    entity->materials[entity->nmaterials++] = brick;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "wall(2)", 4.0f, -0.5f, 8.0f, &entity);
    entity->mesh = wall_mesh;
    entity->materials[entity->nmaterials++] = brick;
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "crate", 0.0f, 0.0f, 0.0f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "crate(2)", -1.0f, 0.0f, 0.1f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "crate(3)", -1.0f, 0.0f, -0.9f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "crate(4)", -0.8f, 1.0f, -0.5f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "barrel", 1.5f, 0.0f, 0.0f, &entity);
    entity->mesh = barrel_mesh;
    entity->materials[entity->nmaterials++] = barrel;
    entity->materials[entity->nmaterials++] = barrel_top;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "cactus", 2.0f, -0.5f, -2.5f, &entity);
    entity->mesh = cactus_mesh;
    entity->materials[entity->nmaterials++] = cactus;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "chainlink fence", 0.0f, -0.5f, 0.8f, &entity);
    entity->mesh = chainlink_fence_mesh;
    entity->materials[entity->nmaterials++] = chainlink;
    entity->materials[entity->nmaterials++] = wood;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "car", 7.5f, -0.5f, 0.0f, &entity);
    entity->mesh = car_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entity->materials[entity->nmaterials++] = wood;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init(sbox, "tommy gun", 1.5f, 0.7f, 0.0f, &entity);
    entity->mesh = tommy_gun_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entity->materials[entity->nmaterials++] = wood;
    entity->is_viewmodel = true;
    entlist_add(sbox, &map->entlist, entity);
}

void map_free(sbox_t* sbox, map_t* map) {
    entlist_free(sbox, &map->entlist);
}

static void send_to_renderer(sbox_t* sbox, map_t* map) {
    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->is_viewmodel) continue;

        drawcall_t drawcall;
        drawcall.mesh = entity->mesh;
        drawcall.nmaterials = entity->nmaterials;
        memcpy(drawcall.materials, entity->materials, sizeof(material_t*) * entity->nmaterials);

        glm_mat4_identity(drawcall.model);
        glm_translate(drawcall.model, entity->position);
        glm_quat_rotate(drawcall.model, entity->rotation, drawcall.model);

        drawcall.dist_to_camera = 0.0f;

        drawcall.is_translucent = false;
		for (int i = 0; i < drawcall.nmaterials; i++) {
			if (drawcall.materials[i]->is_translucent) {
				drawcall.is_translucent = true;
				break;
			}
		}

        r_add_drawcall(&sbox->renderer, drawcall);
    }
}

void map_tick(sbox_t* sbox, map_t* map) {
	entlist_tick(sbox, &map->entlist);
    send_to_renderer(sbox, map);
}
