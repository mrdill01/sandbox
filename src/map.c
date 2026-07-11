#include "map.h"
#include "sbox.h"

void map_load(sbox_t* sbox, map_t* map) {
	entlist_init(sbox, &map->entlist);

    mesh_t* floor_mesh = mesh_load(sbox, "res/meshes/floor.obj");
    mesh_t* wall_mesh = mesh_load(sbox, "res/meshes/wall.obj");
    mesh_t* crate_mesh = mesh_load(sbox, "res/meshes/crate.obj");
    mesh_t* crate2_mesh = mesh_load(sbox, "res/meshes/crate2.obj");
    mesh_t* crate3_mesh = mesh_load(sbox, "res/meshes/crate3.obj");
    mesh_t* crate4_mesh = mesh_load(sbox, "res/meshes/crate4.obj");
    mesh_t* wood_panel_mesh = mesh_load(sbox, "res/meshes/wood_panel.obj");
    mesh_t* metal_panel_mesh = mesh_load(sbox, "res/meshes/metal_panel.obj");
    mesh_t* barrel_mesh = mesh_load(sbox, "res/meshes/barrel.obj");
    mesh_t* cactus_mesh = mesh_load(sbox, "res/meshes/cactus.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(sbox, "res/meshes/chainlink_fence.obj");
    mesh_t* container_mesh = mesh_load(sbox, "res/meshes/container.obj");
    mesh_t* streetlight_mesh = mesh_load(sbox, "res/meshes/streetlight.obj");
    mesh_t* table_mesh = mesh_load(sbox, "res/meshes/table.obj");
    mesh_t* table2_mesh = mesh_load(sbox, "res/meshes/table2.obj");
    mesh_t* bench_mesh = mesh_load(sbox, "res/meshes/bench.obj");
    mesh_t* rock_mesh = mesh_load(sbox, "res/meshes/rock.obj");
    mesh_t* stone_wall_mesh = mesh_load(sbox, "res/meshes/stone_wall.obj");
    mesh_t* water_mesh = mesh_load(sbox, "res/meshes/water.obj");
    mesh_t* car_mesh = mesh_load(sbox, "res/meshes/car.obj");
    mesh_t* helicopter_mesh = mesh_load(sbox, "res/meshes/helicopter.obj");
    mesh_t* tommy_gun_mesh = mesh_load(sbox, "res/meshes/tommy_gun.obj");
    mesh_t* vinyl_mesh = mesh_load(sbox, "res/meshes/vinyl.obj");
    
    material_t* crate = material_load(sbox,
        "res/textures/crate.png",
        "res/textures/crate_r.png",
        "res/textures/crate_n.png",
        1, 1, false, PHYSMAT_WOOD);

    material_t* crate2 = material_load(sbox,
        "res/textures/crate2.png",
        "res/textures/crate2_r.png",
        "res/textures/crate2_n.png",
        1, 1, false, PHYSMAT_WOOD);

    material_t* container = material_load(sbox,
        "res/textures/container.png",
        "res/textures/container_r.png",
        "res/textures/container_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* chainlink = material_load(sbox,
        "res/textures/chainlink.png",
        "res/textures/chainlink_r.png",
        "res/textures/chainlink_n.png",
        20, 20, true, PHYSMAT_METAL);

    material_t* metal = material_load(sbox,
        "res/textures/metal.png",
        "res/textures/metal_r.png",
        "res/textures/metal_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* metal2 = material_load(sbox,
        "res/textures/metal2.png",
        "res/textures/metal2_r.png",
        "res/textures/metal2_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* metal3 = material_load(sbox,
        "res/textures/metal3.png",
        "res/textures/metal3_r.png",
        "res/textures/metal3_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* barrel = material_load(sbox,
        "res/textures/barrel.png",
        "res/textures/barrel_r.png",
        "res/textures/barrel_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* barrel_top = material_load(sbox,
        "res/textures/barrel_top.png",
        "res/textures/barrel_top_r.png",
        "res/textures/barrel_top_n.png",
        1, 1, false, PHYSMAT_METAL);

    material_t* wood = material_load(sbox,
        "res/textures/wood.png",
        "res/textures/wood_r.png",
        "res/textures/wood_n.png",
        1, 1, false, PHYSMAT_WOOD);

    material_t* wood2 = material_load(sbox,
        "res/textures/wood2.png",
        "res/textures/wood2_r.png",
        "res/textures/wood2_n.png",
        2, 2, false, PHYSMAT_WOOD);

    material_t* wood3 = material_load(sbox,
        "res/textures/wood3.png",
        "res/textures/wood3_r.png",
        "res/textures/wood3_n.png",
        1, 1, false, PHYSMAT_WOOD);

    material_t* wood4 = material_load(sbox,
        "res/textures/wood4.png",
        "res/textures/wood4_r.png",
        "res/textures/wood4_n.png",
        2, 2, false, PHYSMAT_METAL);

    material_t* wood5 = material_load(sbox,
        "res/textures/wood5.png",
        "res/textures/wood5_r.png",
        "res/textures/wood5_n.png",
        1, 1, false, PHYSMAT_WOOD);

    material_t* brick = material_load(sbox,
        "res/textures/brick.png",
        "res/textures/brick_r.png",
        "res/textures/brick_n.png",
        6, 6, false, PHYSMAT_STONE);

    material_t* brick2 = material_load(sbox,
        "res/textures/brick2.png",
        "res/textures/brick2_r.png",
        "res/textures/brick2_n.png",
        4, 4, false, PHYSMAT_STONE);

    material_t* tile = material_load(sbox,
        "res/textures/tile.png",
        "res/textures/tile_r.png",
        "res/textures/tile_n.png",
        2, 2, false, PHYSMAT_STONE);

    material_t* tile2 = material_load(sbox,
        "res/textures/tile2.png",
        "res/textures/tile2_r.png",
        "res/textures/tile2_n.png",
        1, 1, false, PHYSMAT_STONE);

    material_t* cactus = material_load(sbox,
        "res/textures/cactus.png",
        "res/textures/cactus_r.png",
        "res/textures/cactus_n.png",
        3, 3, false, PHYSMAT_VEGETATION);

    material_t* rock = material_load(sbox,
        "res/textures/rock.png",
        "res/textures/rock_r.png",
        "res/textures/rock_n.png",
        1, 1, false, PHYSMAT_STONE);

    material_t* stone = material_load(sbox,
        "res/textures/stone.png",
        "res/textures/stone_r.png",
        "res/textures/stone_n.png",
        1, 1, false, PHYSMAT_STONE);

    material_t* water = material_load(sbox,
        "res/textures/water.png",
        "res/textures/water_r.png",
        "res/textures/water_n.png",
        1, 1, false, PHYSMAT_WATER);
    water->is_water = true;

    material_t* light = material_load(sbox,
        "res/textures/light.png",
        "res/textures/light_r.png",
        "res/textures/light_n.png",
        1, 1, false, PHYSMAT_METAL);

    entity_t* entity;
    entity_init_prop(sbox, "floor", 0.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(2)", 0.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, metal2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(3)", 16.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(4)", 16.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(5)", 16.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, tile, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(6)", 8.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, metal2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(7)", 8.0f, -0.5f, 0.0f,floor_mesh,  &entity);
    entity_prop_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "floor(8)", 8.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_prop_set_material(sbox, entity, tile, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall", 0.0f, -0.5f, 4.0f, wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, brick, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall(2)", 4.0f, -0.5f, 8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_prop_set_material(sbox, entity, brick, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall(3)", -4.0f, -0.5f, 0.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_prop_set_material(sbox, entity, brick2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wall(4)", -4.0f, -0.5f, -8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_prop_set_material(sbox, entity, brick2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate", 0.0f, 0.0f, 0.0f, crate_mesh, &entity);
    entity_prop_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(2)", -1.0f, 0.0f, 0.1f, crate_mesh, &entity);
    entity_prop_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(3)", -1.0f, 0.0f, -0.9f, crate_mesh, &entity);
    entity_prop_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "crate(4)", -0.8f, 1.0f, -0.5f, crate_mesh, &entity);
    entity_prop_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "big crate", -3.0f, 0.5f, 3.0f, crate2_mesh, &entity);
    entity_prop_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "big crate(2)", -3.6f, -0.5f, 0.0f, crate3_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood3, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "big crate(2)", -1.0f, -0.5f, 3.5f, crate4_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood4, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "metal panel", -1.275f, -0.5f, 3.0f, metal_panel_mesh, &entity);
    entity_prop_set_material(sbox, entity, metal3, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "table", 2.0f, -0.5f, 3.25f, table_mesh, &entity);
    entity_prop_set_material(sbox, entity, tile2, 0);
    entity_prop_set_material(sbox, entity, metal, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "barrel", 1.5f, 0.0f, 0.0f, barrel_mesh, &entity);
    entity_prop_set_material(sbox, entity, barrel, 0);
    entity_prop_set_material(sbox, entity, barrel_top, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "cactus", 2.0f, -0.5f, -4.5f, cactus_mesh, &entity);
    entity_prop_set_material(sbox, entity, cactus, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "rock", 3.5f, -0.5f, -6.5f, rock_mesh, &entity);
    entity_prop_set_material(sbox, entity, rock, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wood panel", 3.5f, -0.5f, -9.0f, wood_panel_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood5, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wood panel(2)", 2.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood5, 0);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "wood panel(3)", 5.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood5, 0);
    glm_quat(entity->rotation, rad(-45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall", 12.0f, -0.5f, -11.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall(2)", 12.0f, -0.5f, -9.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall(3)", 12.0f, -0.5f, -7.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall(4)", 12.0f, -0.5f, -5.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall(5)", 12.0f, -0.5f, -1.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "stone wall(6)", 12.0f, -0.5f, 1.0f, stone_wall_mesh, &entity);
    entity_prop_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "chainlink fence", 0.0f, -0.5f, 0.8f, chainlink_fence_mesh, &entity);
    entity_prop_set_material(sbox, entity, chainlink, 0);
    entity_prop_set_material(sbox, entity, wood, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "container", -2.5f, -0.5f, -8.0f, container_mesh, &entity);
    entity_prop_set_material(sbox, entity, container, 0);
    entity->data.prop.collision_enabled = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "table(2)", 9.0f, -0.5f, -7.0f, table2_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "bench(1)", 9.0f, -0.5f, -5.8f, bench_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "bench(2)", 9.0f, -0.5f, -8.2f, bench_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "car", 8.0f, -0.5f, 0.0f, car_mesh, &entity);
    glm_quat(entity->rotation, rad(-45.0f), 0.0f, 1.0f, 0.0f);
    entity_prop_set_material(sbox, entity, metal, 0);
    entity_prop_set_material(sbox, entity, wood, 1);
    entity_prop_set_material(sbox, entity, metal, 2);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "helicopter", 8.0f, -0.5f, 8.0f, helicopter_mesh, &entity);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entity_prop_set_material(sbox, entity, metal, 0);
    entity_prop_set_material(sbox, entity, wood, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "water", 0.0f, -1.0f, 0.0f, water_mesh, &entity);
    entity_prop_set_material(sbox, entity, water, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "underwater", 0.0f, -2.5f, 0.0f, water_mesh, &entity);
    entity_prop_set_material(sbox, entity, tile, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "tommy gun", 1.5f, 0.7f, 0.0f, tommy_gun_mesh, &entity);
    entity_prop_set_material(sbox, entity, wood, 0);
    entity->data.prop.is_viewmodel = true;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "vinyl #1", 2.0f, 0.4f, 3.25f, vinyl_mesh, &entity);
    entity_prop_set_material(sbox, entity, water, 0);
    entity->data.prop.is_pickup = true;
    entlist_add(sbox, &map->entlist, entity);

    vec3 color = {32.0f, 32.0f, 32.0f};
    entity_init_light(sbox, "point light", 0.0f, 1.5f, -2.5f, color, &entity);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_prop(sbox, "streetlight", 0.0f, -0.5f, -2.5f, streetlight_mesh, &entity);
    entity_prop_set_material(sbox, entity, metal, 0);
    entity_prop_set_material(sbox, entity, light, 1);
    entlist_add(sbox, &map->entlist, entity);

    vec3 color2 = {0.0f, 32.0f, 32.0f};
    entity_init_light(sbox, "point light(2)", 8.0f, 3.5f, 8.0f, color2, &entity);
    entlist_add(sbox, &map->entlist, entity);

    vec3 color3 = {32.0f, 32.0f, 0.0f};
    entity_init_light(sbox, "point light(3)", 0.0f, 1.5f, -8.0f, color3, &entity);
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
        if (entity->type != ENTITY_PROP) continue;
        if (entity->data.prop.is_viewmodel) continue;

        drawcall_t drawcall;
        drawcall.mesh = entity->data.prop.mesh;
        memcpy(drawcall.materials, entity->data.prop.materials,
            sizeof(material_t*) * MAX_MATERIALS);

        int n = 0;
        for (int i = 0; i < MAX_MATERIALS; i++)
            if (entity->data.prop.materials[i])
                n++;

        if (n != entity->data.prop.mesh->nmaterials) {
            error(sbox, "entity %s doesn't have the proper number of materials (have %d, need %d)",
                entity->name, n, entity->data.prop.mesh->nmaterials);
            return;
        }

        glm_mat4_identity(drawcall.model);
        glm_translate(drawcall.model, entity->position);
        glm_scale(drawcall.model, entity->scale);
        glm_quat_rotate(drawcall.model, entity->rotation, drawcall.model);

        drawcall.dist_to_camera = 0.0f;

        drawcall.is_translucent = false;
		for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall.materials[i];
            if (!material) continue;
			if (material->is_translucent) {
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
