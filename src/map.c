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
    mesh_t* helicopter_mesh = mesh_load(sbox, "res/meshes/helicopter.obj");
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

    material_t* tile = material_load(sbox,
        "res/textures/tile.png",
        "res/textures/tile_r.png",
        "res/textures/tile_n.png",
        2, 2, false);

    material_t* cactus = material_load(sbox,
        "res/textures/cactus.png",
        "res/textures/cactus_r.png",
        "res/textures/cactus_n.png",
        3, 3, false);

    entity_t* entity;
    entity_init_prop(sbox, "floor", 0.0f, -0.5f, 0.0f, &entity);
    entity->data.prop.mesh = floor_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(2)", 8.0f, -0.5f, 0.0f, &entity);
    entity->data.prop.mesh = floor_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(3)", 8.0f, -0.5f, 8.0f, &entity);
    entity->data.prop.mesh = floor_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = tile;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall", 0.0f, -0.5f, 4.0f, &entity);
    entity->data.prop.mesh = wall_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = brick;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall(2)", 4.0f, -0.5f, 8.0f, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity->data.prop.mesh = wall_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = brick;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate", 0.0f, 0.0f, 0.0f, &entity);
    entity->data.prop.mesh = crate_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(2)", -1.0f, 0.0f, 0.1f, &entity);
    entity->data.prop.mesh = crate_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(3)", -1.0f, 0.0f, -0.9f, &entity);
    entity->data.prop.mesh = crate_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(4)", -0.8f, 1.0f, -0.5f, &entity);
    entity->data.prop.mesh = crate_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = crate;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "barrel", 1.5f, 0.0f, 0.0f, &entity);
    entity->data.prop.mesh = barrel_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = barrel;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = barrel_top;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "cactus", 2.0f, -0.5f, -3.5f, &entity);
    entity->data.prop.mesh = cactus_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = cactus;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "chainlink fence", 0.0f, -0.5f, 0.8f, &entity);
    entity->data.prop.mesh = chainlink_fence_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = chainlink;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = wood;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "car", 8.0f, -0.5f, 0.0f, &entity);
    glm_quat(entity->rotation, rad(-45.0f), 0.0f, 1.0f, 0.0f);
    entity->data.prop.mesh = car_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = wood;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "helicopter", 8.0f, -0.5f, 8.0f, &entity);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entity->data.prop.mesh = helicopter_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = wood;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "tommy gun", 1.5f, 0.7f, 0.0f, &entity);
    entity->data.prop.mesh = tommy_gun_mesh;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = metal;
    entity->data.prop.materials[entity->data.prop.nmaterials++] = wood;
    entity->data.prop.is_viewmodel = true;
    entlist_add(sbox, &map->entlist, entity);

    vec3 color = {32.0f, 32.0f, 32.0f};
    entity_init_light(sbox, "point light", 1.5f, 1.5f, -1.5f, color, &entity);
    entlist_add(sbox, &map->entlist, entity);

    vec3 color2 = {0.0f, 32.0f, 32.0f};
    entity_init_light(sbox, "point light(2)", 8.0f, 3.5f, 8.0f, color2, &entity);
    entlist_add(sbox, &map->entlist, entity);

    const char* paths[6] = {
        "res/textures/sky_right.png",
        "res/textures/sky_left.png",
        "res/textures/sky_top.png",
        "res/textures/sky_bottom.png",
        "res/textures/sky_front.png",
        "res/textures/sky_back.png",
    };
    map->skybox = texture_load_cubemap(sbox, paths);
}

void map_free(sbox_t* sbox, map_t* map) {
    entlist_free(sbox, &map->entlist);
}

static void send_to_renderer(sbox_t* sbox, map_t* map) {
    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENT_PROP) continue;
        if (entity->data.prop.is_viewmodel) continue;

        drawcall_t drawcall;
        drawcall.mesh = entity->data.prop.mesh;
        drawcall.nmaterials = entity->data.prop.nmaterials;
        memcpy(drawcall.materials, entity->data.prop.materials,
            sizeof(material_t*) * entity->data.prop.nmaterials);

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
