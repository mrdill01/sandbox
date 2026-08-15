#include "map.h"
#include "quark.h"

void map_init(quark_t* quark, map_t* map) {
    map->is_loaded = false;
    map->skybox = NULL;
    map->coins = 0;
}

void map_load(quark_t* quark, map_t* map) {
    if (map->is_loaded)
	    map_free(quark, &quark->map);
    info(quark, "loading map...");
    srand(sv_random_seed.value);
    
	entlist_init(quark, &map->entlist);

    mesh_t* floor_mesh = mesh_load(quark, "res/meshes/floor.obj");
    mesh_t* wall_mesh = mesh_load(quark, "res/meshes/wall.obj");
    mesh_t* crate_mesh = mesh_load(quark, "res/meshes/crate.obj");
    mesh_t* crate2_mesh = mesh_load(quark, "res/meshes/crate2.obj");
    mesh_t* crate3_mesh = mesh_load(quark, "res/meshes/crate3.obj");
    mesh_t* crate4_mesh = mesh_load(quark, "res/meshes/crate4.obj");
    mesh_t* wood_panel_mesh = mesh_load(quark, "res/meshes/wood_panel.obj");
    mesh_t* wood_frame_mesh = mesh_load(quark, "res/meshes/wood_frame.obj");
    mesh_t* wood_frame_doorway_mesh = mesh_load(quark, "res/meshes/wood_frame_doorway.obj");
    mesh_t* wood_frame_window_mesh = mesh_load(quark, "res/meshes/wood_frame_window.obj");
    mesh_t* stairs_mesh = mesh_load(quark, "res/meshes/stairs.obj");
    mesh_t* metal_panel_mesh = mesh_load(quark, "res/meshes/metal_panel.obj");
    mesh_t* barrel_mesh = mesh_load(quark, "res/meshes/barrel.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(quark, "res/meshes/chainlink_fence.obj");
    mesh_t* container_mesh = mesh_load(quark, "res/meshes/container.obj");
    mesh_t* streetlight_mesh = mesh_load(quark, "res/meshes/streetlight.obj");
    mesh_t* table_mesh = mesh_load(quark, "res/meshes/table.obj");
    mesh_t* table2_mesh = mesh_load(quark, "res/meshes/table2.obj");
    mesh_t* bench_mesh = mesh_load(quark, "res/meshes/bench.obj");
    mesh_t* stone_wall_mesh = mesh_load(quark, "res/meshes/stone_wall.obj");
    mesh_t* bridge_mesh = mesh_load(quark, "res/meshes/bridge.obj");
    mesh_t* water_mesh = mesh_load(quark, "res/meshes/water.obj");
    mesh_t* underwater_mesh = mesh_load(quark, "res/meshes/underwater.obj");
    mesh_t* car_mesh = mesh_load(quark, "res/meshes/car.obj");
    mesh_t* helicopter_mesh = mesh_load(quark, "res/meshes/helicopter.obj");
    mesh_t* ship_mesh = mesh_load(quark, "res/meshes/ship.obj");
    mesh_t* dock_mesh = mesh_load(quark, "res/meshes/dock.obj");
    mesh_t* vinyl_mesh = mesh_load(quark, "res/meshes/vinyl.obj");
    mesh_t* cactus_mesh = mesh_load(quark, "res/meshes/nature/cactus.obj");
    mesh_t* rock_mesh = mesh_load(quark, "res/meshes/nature/rock.obj");
    mesh_t* hedge_mesh = mesh_load(quark, "res/meshes/nature/hedge.obj");
    mesh_t* bush_mesh = mesh_load(quark, "res/meshes/nature/bush.obj");
    mesh_t* bush2_mesh = mesh_load(quark, "res/meshes/nature/bush2.obj");
    mesh_t* tree_mesh = mesh_load(quark, "res/meshes/nature/tree.obj");
    mesh_t* grass_mesh = mesh_load(quark, "res/meshes/nature/grass.obj");
    mesh_t* pipe_mesh = mesh_load(quark, "res/meshes/pipe.obj");
    mesh_t* pipe_bend_mesh = mesh_load(quark, "res/meshes/pipe_bend.obj");
    mesh_t* coin_mesh = mesh_load(quark, "res/meshes/items/coin.obj");
    
    material_t* crate = material_load(quark,
        "crate",
        "res/textures/materials/crate.png",
        "res/textures/materials/crate_r.png",
        "res/textures/materials/crate_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* crate2 = material_load(quark,
        "crate2",
        "res/textures/materials/crate2.png",
        "res/textures/materials/crate2_r.png",
        "res/textures/materials/crate2_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* container = material_load(quark,
        "container",
        "res/textures/materials/container.png",
        "res/textures/materials/container_r.png",
        "res/textures/materials/container_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* chainlink = material_load(quark,
        "chainlink",
        "res/textures/materials/chainlink.png",
        "res/textures/materials/chainlink_r.png",
        "res/textures/materials/chainlink_n.png",
        10, 10, true, PHYS_MAT_METAL);

    material_t* metal = material_load(quark,
        "metal",
        "res/textures/materials/metal.png",
        "res/textures/materials/metal_r.png",
        "res/textures/materials/metal_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* metal2 = material_load(quark,
        "metal2",
        "res/textures/materials/metal2.png",
        "res/textures/materials/metal2_r.png",
        "res/textures/materials/metal2_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* metal3 = material_load(quark,
        "metal3",
        "res/textures/materials/metal3.png",
        "res/textures/materials/metal3_r.png",
        "res/textures/materials/metal3_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* concrete = material_load(quark,
        "concrete",
        "res/textures/materials/concrete.png",
        "res/textures/materials/concrete_r.png",
        "res/textures/materials/concrete_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* concrete2 = material_load(quark,
        "concrete2",
        "res/textures/materials/concrete2.png",
        "res/textures/materials/concrete2_r.png",
        "res/textures/materials/concrete2_n.png",
        2, 2, false, PHYS_MAT_STONE);

    material_t* barrel = material_load(quark,
        "barrel",
        "res/textures/materials/barrel.png",
        "res/textures/materials/barrel_r.png",
        "res/textures/materials/barrel_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* barrel_top = material_load(quark,
        "barrel_top",
        "res/textures/materials/barrel_top.png",
        "res/textures/materials/barrel_top_r.png",
        "res/textures/materials/barrel_top_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* wood = material_load(quark,
        "wood",
        "res/textures/materials/wood.png",
        "res/textures/materials/wood_r.png",
        "res/textures/materials/wood_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* wood2 = material_load(quark,
        "wood2",
        "res/textures/materials/wood2.png",
        "res/textures/materials/wood2_r.png",
        "res/textures/materials/wood2_n.png",
        1.5f, 1.5f, false, PHYS_MAT_WOOD);

    material_t* wood3 = material_load(quark,
        "wood3",
        "res/textures/materials/wood3.png",
        "res/textures/materials/wood3_r.png",
        "res/textures/materials/wood3_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* wood4 = material_load(quark,
        "wood4",
        "res/textures/materials/wood4.png",
        "res/textures/materials/wood4_r.png",
        "res/textures/materials/wood4_n.png",
        2, 2, false, PHYS_MAT_WOOD);

    material_t* wood5 = material_load(quark,
        "wood5",
        "res/textures/materials/wood5.png",
        "res/textures/materials/wood5_r.png",
        "res/textures/materials/wood5_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* brick = material_load(quark,
        "brick",
        "res/textures/materials/brick.png",
        "res/textures/materials/brick_r.png",
        "res/textures/materials/brick_n.png",
        4, 4, false, PHYS_MAT_STONE);

    material_t* brick2 = material_load(quark,
        "brick2",
        "res/textures/materials/brick2.png",
        "res/textures/materials/brick2_r.png",
        "res/textures/materials/brick2_n.png",
        4, 4, false, PHYS_MAT_STONE);

    material_t* tile = material_load(quark,
        "tile",
        "res/textures/materials/tile.png",
        "res/textures/materials/tile_r.png",
        "res/textures/materials/tile_n.png",
        2, 2, false, PHYS_MAT_STONE);

    material_t* tile2 = material_load(quark,
        "tile2",
        "res/textures/materials/tile2.png",
        "res/textures/materials/tile2_r.png",
        "res/textures/materials/tile2_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* cactus = material_load(quark,
        "cactus",
        "res/textures/materials/cactus.png",
        "res/textures/materials/cactus_r.png",
        "res/textures/materials/cactus_n.png",
        3, 3, false, PHYS_MAT_GRASS);
    cactus->wind_factor = 1.0f;

    material_t* cactus2 = material_load(quark,
        "cactus2",
        "res/textures/materials/cactus2.png",
        "res/textures/materials/cactus2_r.png",
        "res/textures/materials/cactus2_n.png",
        1, 1, true, PHYS_MAT_GRASS);
    cactus2->wind_factor = 1.0f;

    material_t* grass = material_load(quark,
        "grass",
        "res/textures/materials/grass.png",
        "res/textures/materials/grass_r.png",
        "res/textures/materials/grass_n.png",
        1, 1, false, PHYS_MAT_GRASS);

    material_t* leaves = material_load(quark,
        "leaves",
        "res/textures/materials/leaves.png",
        "res/textures/materials/leaves_r.png",
        "res/textures/materials/leaves_n.png",
        1, 1, false, PHYS_MAT_GRASS);
    leaves->wind_factor = 1.0f;

    material_t* tall_grass = material_load(quark,
        "tall_grass",
        "res/textures/materials/tall_grass.png",
        "res/textures/materials/tall_grass.png",
        "res/textures/materials/tall_grass.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass->wind_factor = 1.0f;

    material_t* tall_grass2 = material_load(quark,
        "tall_grass2",
        "res/textures/materials/tall_grass2.png",
        "res/textures/materials/tall_grass2.png",
        "res/textures/materials/tall_grass2.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass2->wind_factor = 1.0f;

    material_t* tall_grass3 = material_load(quark,
        "tall_grass3",
        "res/textures/materials/tall_grass3.png",
        "res/textures/materials/tall_grass3.png",
        "res/textures/materials/tall_grass3.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass3->wind_factor = 1.0f;

    material_t* tall_grass4 = material_load(quark,
        "tall_grass4",
        "res/textures/materials/tall_grass4.png",
        "res/textures/materials/tall_grass4.png",
        "res/textures/materials/tall_grass4.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass4->wind_factor = 1.0f;

    material_t* rock = material_load(quark,
        "rock",
        "res/textures/materials/rock.png",
        "res/textures/materials/rock_r.png",
        "res/textures/materials/rock_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* stone = material_load(quark,
        "stone",
        "res/textures/materials/stone.png",
        "res/textures/materials/stone_r.png",
        "res/textures/materials/stone_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* water = material_load(quark,
        "water",
        "res/textures/materials/water.png",
        "res/textures/materials/water_r.png",
        "res/textures/materials/water_n.png",
        1, 1, true, PHYS_MAT_WATER);
    water->is_water = true;
    water->scroll_speed = 0.05f;

    material_t* sand = material_load(quark,
        "sand",
        "res/textures/materials/sand.png",
        "res/textures/materials/sand_r.png",
        "res/textures/materials/sand_n.png",
        1, 1, false, PHYS_MAT_SAND);

    material_t* tire = material_load(quark,
        "tire",
        "res/textures/materials/tire.png",
        "res/textures/materials/tire_r.png",
        "res/textures/materials/tire_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* glass = material_load(quark,
        "glass",
        "res/textures/materials/glass.png",
        "res/textures/materials/glass_r.png",
        "res/textures/materials/glass_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* light = material_load(quark,
        "light",
        "res/textures/materials/light.png",
        "res/textures/materials/light_r.png",
        "res/textures/materials/light_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* test = material_load(quark,
        "test",
        "res/textures/materials/test.png",
        "res/textures/materials/test_r.png",
        "res/textures/materials/test_n.png",
        2, 2, false, PHYS_MAT_METAL);

    material_t* coin = material_load(quark,
        "coin",
        "res/textures/materials/coin.png",
        "res/textures/materials/coin_r.png",
        "res/textures/materials/coin_n.png",
        1, 1, false, PHYS_MAT_METAL);

    entity_t* entity;
    entity_init_mesh(quark, "floor", 0.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(2)", 0.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(3)", 16.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(4)", 16.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(5)", 24.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(6)", 24.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(7)", 16.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(7)", 16.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(8)", 24.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(8)", 24.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(8)", 16.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(8)", 24.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, grass, 0);
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 256; i++) {
        float x = random(12.0f, 28.0f);
        float y = -0.5f;
        float z = random(12.0f, 28.0f);
        entity_init_mesh(quark, "grass", x, y, z, grass_mesh, &entity);

        material_t* materials[] = {
            tall_grass, tall_grass2, tall_grass3, tall_grass4
        };

        material_t* material = materials[
            (size_t)random(0.0f, sizeof(materials) / sizeof(materials[0]))];
        entity_mesh_set_material(quark, entity, material, 0);

        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        entity->data.mesh.enable_collision = false;
        glm_vec3_scale(entity->scale, random(0.5f, 1.5f), entity->scale);
        entlist_add(quark, &map->entlist, entity);
    }

    for (int i = 0; i < 10; i++) {
        float x = random(12.0f, 28.0f);
        float y = -0.5f;
        float z = random(12.0f, 28.0f);
        entity_init_mesh(quark, "tree", x, y, z, tree_mesh, &entity);
        entity_mesh_set_material(quark, entity, leaves, 0);
        entity_mesh_set_material(quark, entity, wood, 1);

        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        glm_vec3_scale(entity->scale, random(0.75f, 1.5f), entity->scale);
        entlist_add(quark, &map->entlist, entity);
    }

    for (int i = 0; i < 48; i++) {
        float x = random(28.0f, 44.0f);
        float y = -0.5f;
        float z = random(12.0f, 28.0f);

        mesh_t* mesh = cactus_mesh;
        material_t* material = cactus;
        if (random(0.0f, 1.0f) >= 0.15f) {
            mesh = grass_mesh;
            material = cactus2;
        }

        entity_init_mesh(quark, "cactus", x, y, z, mesh, &entity);
        entity_mesh_set_material(quark, entity, material, 0);
        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        glm_vec3_scale(entity->scale, random(0.5f, 1.5f), entity->scale);
        entlist_add(quark, &map->entlist, entity);
    }

    entity_init_mesh(quark, "floor(8)", 40.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, sand, 0);
    entlist_add(quark, &map->entlist, entity);

     entity_init_mesh(quark, "floor(8)", 32.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, sand, 0);
    entlist_add(quark, &map->entlist, entity);

     entity_init_mesh(quark, "floor(8)", 40.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, sand, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(9)", 32.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(10)", 32.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(11)", 32.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(12)", 40.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(13)", 40.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(14)", 40.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(12)", 48.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(13)", 48.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(14)", 48.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(15)", 8.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(16)", 8.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(17)", 8.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(18)", 32.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, test, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(19)", 36.0f, 3.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, test, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 0.0f, 1.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(20)", 8.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, tile, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(21)", 0.0f, 2.0f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(22)", -8.0f, 2.0f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(23)", -8.0f, 2.0f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "floor(24)", -8.0f, 2.0f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wall", 0.0f, -0.5f, 4.0f, wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, brick, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wall(2)", 4.0f, -0.5f, 8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(quark, entity, brick, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wall(3)", -4.0f, -0.5f, 0.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(quark, entity, brick2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wall(4)", -4.0f, -0.5f, -8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(quark, entity, brick2, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "crate", 0.0f, 0.0f, 0.0f, crate_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "crate(2)", -1.0f, 0.0f, 0.1f, crate_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "crate(3)", -1.0f, 0.0f, -0.9f, crate_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "crate(4)", -0.8f, 1.0f, -0.5f, crate_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate", -3.0f, 0.5f, 3.0f, crate2_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate2, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate(2)", -3.6f, -0.5f, 0.0f, crate3_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood3, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate(3)", -1.0f, -0.5f, 3.5f, crate4_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood4, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate(4)", 29.0f, 0.5f, 7.5f, crate2_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate2, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 32; i++) {
        float x = random(7.0f, 30.0f);
        float y = random(-2.5f, 0.0f);
        float z = random(-13.0f, -24.0f);
        entity_init_mesh(quark, "big crate", x, y, z, crate2_mesh, &entity);
        entity_mesh_set_material(quark, entity, crate2, 0);
        entity->data.mesh.enable_physics = true;
        entlist_add(quark, &map->entlist, entity);
    }

    entity_init_mesh(quark, "metal panel", -1.1f, -0.5f, 3.0f, metal_panel_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal3, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "table", 2.0f, -0.5f, 3.25f, table_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood, 0);
    entity_mesh_set_material(quark, entity, tile2, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "barrel", 1.5f, 0.0f, 0.0f, barrel_mesh, &entity);
    entity_mesh_set_material(quark, entity, barrel, 0);
    entity_mesh_set_material(quark, entity, barrel_top, 1);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "cactus", 2.0f, -0.5f, -4.5f, cactus_mesh, &entity);
    entity_mesh_set_material(quark, entity, cactus, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "rock", 3.5f, -0.5f, -6.5f, rock_mesh, &entity);
    entity_mesh_set_material(quark, entity, rock, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood panel", 3.5f, -0.5f, -9.0f, wood_panel_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood panel(2)", 2.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood panel(3)", 5.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    glm_quat(entity->rotation, rad(-45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame", 16.0f + 9.0f, -0.5f, -8.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame window", 16.0f + 9.0f, -0.5f, -5.0f,
        wood_frame_window_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(2)", 16.0f + 12.0f, -0.5f, -2.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(3)", 16.0f + 12.0f, -0.5f, 1.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(4)", 16.0f, -0.5f, -8.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(5)", 16.0f, -0.5f, -5.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame doorway", 16.0f, -0.5f, -2.0f,
        wood_frame_doorway_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame window", 16.0f, -0.5f, 1.0f,
        wood_frame_window_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(7)", 17.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(8)", 20.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(9)", 23.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(9)", 23.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(9)", 26.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(9)", 26.5f, -0.5f, -3.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(10)", 17.5f, -0.5f, -9.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame doorway(2)", 20.5f, -0.5f, -9.5f,
        wood_frame_doorway_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "wood frame(11)", 23.5f, -0.5f, -9.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood5, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    for (int x = 0; x < 4; x++) {
        for (int z = 0; z < 4; z++) {
            if ((x == 2 && z == 3) || (x == 2 && z == 3) || (x == 1 && z == 3))
                continue;
            
            entity_init_mesh(quark,
                "wood frame(ceiling)",
                19.0f + x * 3.0f, 2.5f, -8.0f + z * 3.0f,
                wood_frame_mesh, &entity);
            entity_mesh_set_material(quark, entity, wood5, 0);
            entity_mesh_set_material(quark, entity, wood, 1);
            glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
            glm_quat(entity->rotation, rad(90.0f), 0.0f, 0.0f, 1.0f);
            entlist_add(quark, &map->entlist, entity);
        }
    }

    entity_init_mesh(quark, "stairs", 18.5f, -0.5f, 1.0f, stairs_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stairs", 20.0f, -0.5f, 27.0f, stairs_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 10; i++) {
        entity_init_mesh(quark, "bridge", 20.0f, -0.5f, 36.5f + i * 6.0f, bridge_mesh, &entity);
        entity_mesh_set_material(quark, entity, concrete, 0);
        entlist_add(quark, &map->entlist, entity);
    }
    
    entity_init_mesh(quark, "stone wall", 12.0f, -0.5f, -11.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stone wall(2)", 12.0f, -0.5f, -9.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stone wall(3)", 12.0f, -0.5f, -7.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stone wall(4)", 12.0f, -0.5f, -5.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stone wall(5)", 12.0f, -0.5f, -1.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "stone wall(6)", 12.0f, -0.5f, 1.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(quark, entity, stone, 0);
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 12; i++) {
        if (i == 1 || i == 11) continue;
        entity_init_mesh(quark,
            "stone wall",
            40.0f, -0.5f, -11.0f + i * 2.0f,
            stone_wall_mesh,
            &entity);
        entity_mesh_set_material(quark, entity, stone, 0);
        entlist_add(quark, &map->entlist, entity);
    }

    entity_init_mesh(quark, "crate", 39.3f, 0.0f, -4.5f, crate_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate", 39.0f, 0.5f, -3.0f, crate2_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate2, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate", 36.1f, 0.5f, 4.5f, crate2_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate2, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "big crate", 36.1f, 2.5f, 4.5f, crate2_mesh, &entity);
    entity_mesh_set_material(quark, entity, crate2, 0);
    entity->data.mesh.enable_physics = true;
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 5; i++) {
        entity_init_mesh(quark,
            "chainlink fence",
            35.0f, -0.5f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(quark, entity, chainlink, 0);
        entity_mesh_set_material(quark, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);

        entity_init_mesh(quark,
            "chainlink fence",
            35.0f, 1.5f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(quark, entity, chainlink, 0);
        entity_mesh_set_material(quark, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);

        entity_init_mesh(quark,
            "chainlink fence",
            35.0f, 3.0f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(quark, entity, chainlink, 0);
        entity_mesh_set_material(quark, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);
    }

    for (int i = 0; i < 3; i++) {
        entity_init_mesh(quark, "pipe", 36.0f, 1.5f + i * 4.0f, 11.0f, pipe_mesh, &entity);
        entity_mesh_set_material(quark, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);

        entity_init_mesh(quark, "pipe", 36.0f, 1.5f + i * 4.0f, 9.0f, pipe_mesh, &entity);
        entity_mesh_set_material(quark, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);

        entity_init_mesh(quark, "pipe", 36.0f, 1.5f + i * 4.0f, 7.0f, pipe_mesh, &entity);
        entity_mesh_set_material(quark, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(quark, &map->entlist, entity);
    }

    entity_init_mesh(quark, "chainlink fence", 0.0f, -0.5f, 0.8f, chainlink_fence_mesh, &entity);
    entity_mesh_set_material(quark, entity, chainlink, 0);
    entity_mesh_set_material(quark, entity, wood, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "container", -2.5f, -0.5f, -8.0f, container_mesh, &entity);
    entity_mesh_set_material(quark, entity, container, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe bend", 32.0f, 0.5f, 10.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe", 32.0f, 0.5f, 6.0f, pipe_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe(2)", 32.0f, 0.5f, 2.0f, pipe_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe bend(2)", 32.0f, 0.5f, -2.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 0.0f, 1.0f);
    glm_quat(entity->rotation, rad(180.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe bend(3)", 34.0f, 1.5f, 10.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe(4)", 34.0f, -1.5f, 11.0f, pipe_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe(5)", 34.0f, 1.5f, 6.0f, pipe_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe(6)", 34.0f, 1.5f, 2.0f, pipe_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "pipe bend(4)", 34.0f, 1.5f, -2.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 0.0f, 1.0f);
    glm_quat(entity->rotation, rad(180.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "table(2)", 9.0f, -0.5f, -7.0f, table2_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "bench(1)", 9.0f, -0.5f, -5.8f, bench_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "bench(2)", 9.0f, -0.5f, -8.2f, bench_mesh, &entity);
    entity_mesh_set_material(quark, entity, wood, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_vehicle(quark, "car", (vec3){9.0f, -0.5f, 0.0f}, car_mesh, VEHICLE_CAR, &entity);
    glm_quat(entity->rotation, rad(0.0f), 0.0f, 1.0f, 0.0f);
    entity_vehicle_set_material(quark, entity, wood, 0);
    entity_vehicle_set_material(quark, entity, tire, 1);
    entity_vehicle_set_material(quark, entity, glass, 2);
    entlist_add(quark, &map->entlist, entity);

    entity_init_vehicle(quark, "helicopter", (vec3){8.0f, -0.5f, 16.0f}, helicopter_mesh,
        VEHICLE_HELICOPTER, &entity);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entity_vehicle_set_material(quark, entity, metal, 0);
    entity_vehicle_set_material(quark, entity, glass, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "water", 0.0f, -1.0f, 0.0f, water_mesh, &entity);
    entity_mesh_set_material(quark, entity, water, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "underwater", 0.0f, -2.0f, 0.0f, underwater_mesh, &entity);
    entity_mesh_set_material(quark, entity, sand, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "ship", 0.0f, -0.5f, -32.0f, ship_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "dock", 4.0f, -1.0f, -12.0f, dock_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "dock(2)", 4.0f, -1.0f, -18.0f, dock_mesh, &entity);
    entity_mesh_set_material(quark, entity, concrete, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "vinyl #1", 2.0f, 0.4f, 3.25f, vinyl_mesh, &entity);
    entity_mesh_set_material(quark, entity, water, 0);
    entity->data.mesh.is_pickup = true;
    entlist_add(quark, &map->entlist, entity);

    vec3 sun_dir = {0.0f, -0.999f, 0.0f};
    vec3 sun_color = {6.0f, 5.5f, 5.0f};
    entity_init_sun_light(quark, "sun", 0.0f, 0.0f, 0.0f, sun_dir, sun_color, &entity);
    entlist_add(quark, &map->entlist, entity);

    /*vec3 color = {32.0f, 32.0f, 32.0f};
    entity_init_point_light(quark, "point light", 0.0f, 1.5f, -2.5f, color, &entity);
    entlist_add(quark, &map->entlist, entity);*/

    entity_init_mesh(quark, "streetlight", 0.0f, -0.5f, -2.5f, streetlight_mesh, &entity);
    entity_mesh_set_material(quark, entity, metal, 0);
    entity_mesh_set_material(quark, entity, light, 1);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge", 15.5f, -0.5f, 6.0f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(2)", 15.5f, -0.5f, 10.0f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(3)", 24.5f, -0.5f, 6.0f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(4)", 24.5f, -0.5f, 10.0f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(5)", 23.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(6)", 21.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(7)", 19.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(8)", 17.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(5)", 23.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(6)", 21.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(7)", 19.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(8)", 17.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(9)", 23.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(10)", 21.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(11)", 19.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    entity_init_mesh(quark, "hedge(12)", 17.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(quark, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(quark, &map->entlist, entity);

    for (int i = 0; i < 16; i++) {
        vec3 min = {15.0f + 1.0f, -0.5f, 4.5f + 1.0f};
        vec3 max = {24.5f - 1.0f, -0.5f, 11.5f - 1.0f};
        vec3 pos = {random(min[0], max[0]), random(min[1], max[1]), random(min[2], max[2])};
        mesh_t* mesh = (random(0.0f, 1.0f) > 0.5f) ? bush2_mesh : bush_mesh;

        entity_init_mesh(quark, "bush", pos[0], pos[1], pos[2], mesh, &entity);
        entity_mesh_set_material(quark, entity, leaves, 0);
        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        glm_vec3_scale(entity->scale, random(1.0f, 3.0f), entity->scale);
        entity->data.mesh.enable_collision = false;
        entlist_add(quark, &map->entlist, entity);
    }

    /*for (int i = 0; i < 16; i++) {
        float x = random(-8.0f, 40.0f);
        float z = random(-40.0f, 16.0f);

        vec3 size;
        bbox_get_size(&coin_mesh->bbox, size);

        vec3 start = {x, -0.5f + size[1], z};
        vec3 dir = {0.0f, -1.0f, 0.0f};
        float max_distance = 100.0f;
        trace_result_t trace;

        //r_add_line(quark, &quark->renderer, start, dir, COLOR_GREEN, 5.0f);

        //if (phys_line_trace(quark, start, dir, max_distance, &quark->map.entlist, -1, &trace)) {
        for (int j = -3; j <= 3; j++) {
            for (int k = -3; k <= 3; k++) {
                entity_init_pickup(quark, "coin",
                    (vec3){x + j, start[1] + 0.1f, z + k},
                    coin_mesh, quark->audio.pickup_coin_sound, &entity);
                entity_pickup_set_material(quark, entity, coin, 0);
                entity->data.pickup.is_coin = true;
                entlist_add(quark, &map->entlist, entity);
                map->coins++;
            }
        }
            
        //}
    }*/

    const char* paths[6] = {
        "res/textures/skies/sky_right.png",
        "res/textures/skies/sky_left.png",
        "res/textures/skies/sky_top.png",
        "res/textures/skies/sky_bottom.png",
        "res/textures/skies/sky_front.png",
        "res/textures/skies/sky_back.png",
    };
    map->skybox = texture_load_cubemap(quark, paths);

    info(quark, "map loaded!");
    map->is_loaded = true;
    quark->ui_state = UI_STATE_IN_GAME;
}

void map_free(quark_t* quark, map_t* map) {
    if (!map->is_loaded) return;
    entlist_free(quark, &map->entlist);
    map_init(quark, map);
}

static void send_to_renderer(quark_t* quark, map_t* map) {
    for (size_t i = 0; i < quark->map.entlist.len; i++) {
        entity_t* entity = quark->map.entlist.ents[i];
        if (!entity) continue;

        drawcall_t drawcall = {0};
        if (entity_get_drawcall(quark, entity, &drawcall))
            r_add_drawcall(&quark->renderer, drawcall);
    }
}

void map_tick(quark_t* quark, map_t* map) {
	if (!quark->map.is_loaded) return;

	entlist_tick(quark, &map->entlist);
    send_to_renderer(quark, map);

    for (int i = 0; i < NET_MAX_PLAYERS; i++) {
        player_t* player = quark->players[i];
        if (!player) continue;
        if (player->position[1] < -100.0f)
            player_add_damage(quark, player, player->health);
    }
}
