#include "map.h"
#include "sbox.h"

void map_init(sbox_t* sbox, map_t* map) {
    map->is_loaded = false;
    map->skybox = NULL;
}

void map_load(sbox_t* sbox, map_t* map) {
    info(sbox, "loading map...");
    
	entlist_init(sbox, &map->entlist);

    mesh_t* floor_mesh = mesh_load(sbox, "res/meshes/floor.obj");
    mesh_t* wall_mesh = mesh_load(sbox, "res/meshes/wall.obj");
    mesh_t* crate_mesh = mesh_load(sbox, "res/meshes/crate.obj");
    mesh_t* crate2_mesh = mesh_load(sbox, "res/meshes/crate2.obj");
    mesh_t* crate3_mesh = mesh_load(sbox, "res/meshes/crate3.obj");
    mesh_t* crate4_mesh = mesh_load(sbox, "res/meshes/crate4.obj");
    mesh_t* wood_panel_mesh = mesh_load(sbox, "res/meshes/wood_panel.obj");
    mesh_t* wood_frame_mesh = mesh_load(sbox, "res/meshes/wood_frame.obj");
    mesh_t* wood_frame_doorway_mesh = mesh_load(sbox, "res/meshes/wood_frame_doorway.obj");
    mesh_t* wood_frame_window_mesh = mesh_load(sbox, "res/meshes/wood_frame_window.obj");
    mesh_t* metal_panel_mesh = mesh_load(sbox, "res/meshes/metal_panel.obj");
    mesh_t* barrel_mesh = mesh_load(sbox, "res/meshes/barrel.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(sbox, "res/meshes/chainlink_fence.obj");
    mesh_t* container_mesh = mesh_load(sbox, "res/meshes/container.obj");
    mesh_t* streetlight_mesh = mesh_load(sbox, "res/meshes/streetlight.obj");
    mesh_t* table_mesh = mesh_load(sbox, "res/meshes/table.obj");
    mesh_t* table2_mesh = mesh_load(sbox, "res/meshes/table2.obj");
    mesh_t* bench_mesh = mesh_load(sbox, "res/meshes/bench.obj");
    mesh_t* stone_wall_mesh = mesh_load(sbox, "res/meshes/stone_wall.obj");
    mesh_t* water_mesh = mesh_load(sbox, "res/meshes/water.obj");
    mesh_t* underwater_mesh = mesh_load(sbox, "res/meshes/underwater.obj");
    mesh_t* car_mesh = mesh_load(sbox, "res/meshes/car.obj");
    mesh_t* helicopter_mesh = mesh_load(sbox, "res/meshes/helicopter.obj");
    mesh_t* ship_mesh = mesh_load(sbox, "res/meshes/ship.obj");
    mesh_t* dock_mesh = mesh_load(sbox, "res/meshes/dock.obj");
    mesh_t* vinyl_mesh = mesh_load(sbox, "res/meshes/vinyl.obj");
    mesh_t* cactus_mesh = mesh_load(sbox, "res/meshes/nature/cactus.obj");
    mesh_t* rock_mesh = mesh_load(sbox, "res/meshes/nature/rock.obj");
    mesh_t* hedge_mesh = mesh_load(sbox, "res/meshes/nature/hedge.obj");
    mesh_t* bush_mesh = mesh_load(sbox, "res/meshes/nature/bush.obj");
    mesh_t* bush2_mesh = mesh_load(sbox, "res/meshes/nature/bush2.obj");
    mesh_t* grass_mesh = mesh_load(sbox, "res/meshes/nature/grass.obj");
    mesh_t* pipe_mesh = mesh_load(sbox, "res/meshes/pipe.obj");
    mesh_t* pipe_bend_mesh = mesh_load(sbox, "res/meshes/pipe_bend.obj");
    mesh_t* coin_mesh = mesh_load(sbox, "res/meshes/items/coin.obj");
    
    material_t* crate = material_load(sbox,
        "crate",
        "res/textures/materials/crate.png",
        "res/textures/materials/crate_r.png",
        "res/textures/materials/crate_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* crate2 = material_load(sbox,
        "crate2",
        "res/textures/materials/crate2.png",
        "res/textures/materials/crate2_r.png",
        "res/textures/materials/crate2_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* container = material_load(sbox,
        "container",
        "res/textures/materials/container.png",
        "res/textures/materials/container_r.png",
        "res/textures/materials/container_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* chainlink = material_load(sbox,
        "chainlink",
        "res/textures/materials/chainlink.png",
        "res/textures/materials/chainlink_r.png",
        "res/textures/materials/chainlink_n.png",
        8, 8, true, PHYS_MAT_METAL);

    material_t* metal = material_load(sbox,
        "metal",
        "res/textures/materials/metal.png",
        "res/textures/materials/metal_r.png",
        "res/textures/materials/metal_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* metal2 = material_load(sbox,
        "metal2",
        "res/textures/materials/metal2.png",
        "res/textures/materials/metal2_r.png",
        "res/textures/materials/metal2_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* metal3 = material_load(sbox,
        "metal3",
        "res/textures/materials/metal3.png",
        "res/textures/materials/metal3_r.png",
        "res/textures/materials/metal3_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* concrete = material_load(sbox,
        "concrete",
        "res/textures/materials/concrete.png",
        "res/textures/materials/concrete_r.png",
        "res/textures/materials/concrete_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* concrete2 = material_load(sbox,
        "concrete2",
        "res/textures/materials/concrete2.png",
        "res/textures/materials/concrete2_r.png",
        "res/textures/materials/concrete2_n.png",
        2, 2, false, PHYS_MAT_STONE);

    material_t* barrel = material_load(sbox,
        "barrel",
        "res/textures/materials/barrel.png",
        "res/textures/materials/barrel_r.png",
        "res/textures/materials/barrel_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* barrel_top = material_load(sbox,
        "barrel_top",
        "res/textures/materials/barrel_top.png",
        "res/textures/materials/barrel_top_r.png",
        "res/textures/materials/barrel_top_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* wood = material_load(sbox,
        "wood",
        "res/textures/materials/wood.png",
        "res/textures/materials/wood_r.png",
        "res/textures/materials/wood_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* wood2 = material_load(sbox,
        "wood2",
        "res/textures/materials/wood2.png",
        "res/textures/materials/wood2_r.png",
        "res/textures/materials/wood2_n.png",
        1.5f, 1.5f, false, PHYS_MAT_WOOD);

    material_t* wood3 = material_load(sbox,
        "wood3",
        "res/textures/materials/wood3.png",
        "res/textures/materials/wood3_r.png",
        "res/textures/materials/wood3_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* wood4 = material_load(sbox,
        "wood4",
        "res/textures/materials/wood4.png",
        "res/textures/materials/wood4_r.png",
        "res/textures/materials/wood4_n.png",
        2, 2, false, PHYS_MAT_WOOD);

    material_t* wood5 = material_load(sbox,
        "wood5",
        "res/textures/materials/wood5.png",
        "res/textures/materials/wood5_r.png",
        "res/textures/materials/wood5_n.png",
        1, 1, false, PHYS_MAT_WOOD);

    material_t* brick = material_load(sbox,
        "brick",
        "res/textures/materials/brick.png",
        "res/textures/materials/brick_r.png",
        "res/textures/materials/brick_n.png",
        4, 4, false, PHYS_MAT_STONE);

    material_t* brick2 = material_load(sbox,
        "brick2",
        "res/textures/materials/brick2.png",
        "res/textures/materials/brick2_r.png",
        "res/textures/materials/brick2_n.png",
        4, 4, false, PHYS_MAT_STONE);

    material_t* tile = material_load(sbox,
        "tile",
        "res/textures/materials/tile.png",
        "res/textures/materials/tile_r.png",
        "res/textures/materials/tile_n.png",
        2, 2, false, PHYS_MAT_STONE);

    material_t* tile2 = material_load(sbox,
        "tile2",
        "res/textures/materials/tile2.png",
        "res/textures/materials/tile2_r.png",
        "res/textures/materials/tile2_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* cactus = material_load(sbox,
        "cactus",
        "res/textures/materials/cactus.png",
        "res/textures/materials/cactus_r.png",
        "res/textures/materials/cactus_n.png",
        3, 3, false, PHYS_MAT_GRASS);
    cactus->wind_factor = 1.0f;

    material_t* cactus2 = material_load(sbox,
        "cactus2",
        "res/textures/materials/cactus2.png",
        "res/textures/materials/cactus2_r.png",
        "res/textures/materials/cactus2_n.png",
        1, 1, true, PHYS_MAT_GRASS);
    cactus2->wind_factor = 1.0f;

    material_t* grass = material_load(sbox,
        "grass",
        "res/textures/materials/grass.png",
        "res/textures/materials/grass_r.png",
        "res/textures/materials/grass_n.png",
        1, 1, false, PHYS_MAT_GRASS);

    material_t* leaves = material_load(sbox,
        "leaves",
        "res/textures/materials/leaves.png",
        "res/textures/materials/leaves_r.png",
        "res/textures/materials/leaves_n.png",
        1, 1, false, PHYS_MAT_GRASS);
    leaves->wind_factor = 1.0f;

    material_t* tall_grass = material_load(sbox,
        "tall_grass",
        "res/textures/materials/tall_grass.png",
        "res/textures/materials/tall_grass.png",
        "res/textures/materials/tall_grass.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass->wind_factor = 1.0f;

    material_t* tall_grass2 = material_load(sbox,
        "tall_grass2",
        "res/textures/materials/tall_grass2.png",
        "res/textures/materials/tall_grass2.png",
        "res/textures/materials/tall_grass2.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass2->wind_factor = 1.0f;

    material_t* tall_grass3 = material_load(sbox,
        "tall_grass3",
        "res/textures/materials/tall_grass3.png",
        "res/textures/materials/tall_grass3.png",
        "res/textures/materials/tall_grass3.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass3->wind_factor = 1.0f;

    material_t* tall_grass4 = material_load(sbox,
        "tall_grass4",
        "res/textures/materials/tall_grass4.png",
        "res/textures/materials/tall_grass4.png",
        "res/textures/materials/tall_grass4.png",
        1, 1, true, PHYS_MAT_GRASS);
    tall_grass4->wind_factor = 1.0f;

    material_t* rock = material_load(sbox,
        "rock",
        "res/textures/materials/rock.png",
        "res/textures/materials/rock_r.png",
        "res/textures/materials/rock_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* stone = material_load(sbox,
        "stone",
        "res/textures/materials/stone.png",
        "res/textures/materials/stone_r.png",
        "res/textures/materials/stone_n.png",
        1, 1, false, PHYS_MAT_STONE);

    material_t* water = material_load(sbox,
        "water",
        "res/textures/materials/water.png",
        "res/textures/materials/water_r.png",
        "res/textures/materials/water_n.png",
        1, 1, true, PHYS_MAT_WATER);
    water->is_water = true;
    water->scroll_speed = 0.05f;

    material_t* sand = material_load(sbox,
        "sand",
        "res/textures/materials/sand.png",
        "res/textures/materials/sand_r.png",
        "res/textures/materials/sand_n.png",
        1, 1, false, PHYS_MAT_SAND);

    material_t* light = material_load(sbox,
        "light",
        "res/textures/materials/light.png",
        "res/textures/materials/light_r.png",
        "res/textures/materials/light_n.png",
        1, 1, false, PHYS_MAT_METAL);

    material_t* test = material_load(sbox,
        "test",
        "res/textures/materials/test.png",
        "res/textures/materials/test_r.png",
        "res/textures/materials/test_n.png",
        2, 2, false, PHYS_MAT_METAL);

    material_t* coin = material_load(sbox,
        "coin",
        "res/textures/materials/coin.png",
        "res/textures/materials/coin_r.png",
        "res/textures/materials/coin_n.png",
        1, 1, false, PHYS_MAT_METAL);

    entity_t* entity;
    entity_init_mesh(sbox, "floor", 0.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(2)", 0.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(3)", 16.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(4)", 16.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(5)", 24.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(6)", 24.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(7)", 16.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(7)", 16.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(8)", 24.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(8)", 24.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(8)", 16.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(8)", 24.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, grass, 0);
    entlist_add(sbox, &map->entlist, entity);

    for (int i = 0; i < 256; i++) {
        float x = random(12.0f, 28.0f);
        float y = -0.5f;
        float z = random(12.0f, 28.0f);
        entity_init_mesh(sbox, "grass", x, y, z, grass_mesh, &entity);

        material_t* materials[] = {
            tall_grass, tall_grass2, tall_grass3, tall_grass4
        };

        material_t* material = materials[
            (size_t)random(0.0f, sizeof(materials) / sizeof(materials[0]))];
        entity_mesh_set_material(sbox, entity, material, 0);

        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        entity->data.mesh.enable_collision = false;
        glm_vec3_scale(entity->scale, random(0.5f, 1.5f), entity->scale);
        entlist_add(sbox, &map->entlist, entity);
    }

    entity_init_mesh(sbox, "floor(8)", 40.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, sand, 0);
    entlist_add(sbox, &map->entlist, entity);

     entity_init_mesh(sbox, "floor(8)", 32.0f, -0.5f, 24.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, sand, 0);
    entlist_add(sbox, &map->entlist, entity);

     entity_init_mesh(sbox, "floor(8)", 40.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, sand, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(9)", 32.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(10)", 32.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(11)", 32.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(12)", 40.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(13)", 40.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(14)", 40.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(15)", 8.0f, -0.5f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(16)", 8.0f, -0.5f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(17)", 8.0f, -0.5f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(18)", 32.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, test, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(19)", 36.0f, 3.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, test, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 0.0f, 1.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(20)", 8.0f, -0.5f, 16.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, tile, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(21)", 0.0f, 2.0f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(22)", -8.0f, 2.0f, 8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(23)", -8.0f, 2.0f, 0.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "floor(24)", -8.0f, 2.0f, -8.0f, floor_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wall", 0.0f, -0.5f, 4.0f, wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, brick, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wall(2)", 4.0f, -0.5f, 8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(sbox, entity, brick, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wall(3)", -4.0f, -0.5f, 0.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(sbox, entity, brick2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wall(4)", -4.0f, -0.5f, -8.0f, wall_mesh, &entity);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(sbox, entity, brick2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "crate", 0.0f, 0.0f, 0.0f, crate_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "crate(2)", -1.0f, 0.0f, 0.1f, crate_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "crate(3)", -1.0f, 0.0f, -0.9f, crate_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "crate(4)", -0.8f, 1.0f, -0.5f, crate_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate", -3.0f, 0.5f, 3.0f, crate2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate(2)", -3.6f, -0.5f, 0.0f, crate3_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood3, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate(3)", -1.0f, -0.5f, 3.5f, crate4_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood4, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate(4)", 29.0f, 0.5f, 0.5f, crate2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    for (int i = 0; i < 32; i++) {
        float x = random(7.0f, 30.0f);
        float y = random(-2.5f, 0.0f);
        float z = random(-13.0f, -24.0f);
        entity_init_mesh(sbox, "big crate", x, y, z, crate2_mesh, &entity);
        entity_mesh_set_material(sbox, entity, crate2, 0);
        entlist_add(sbox, &map->entlist, entity);
    }

    entity_init_mesh(sbox, "metal panel", -1.1f, -0.5f, 3.0f, metal_panel_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal3, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "table", 2.0f, -0.5f, 3.25f, table_mesh, &entity);
    entity_mesh_set_material(sbox, entity, tile2, 0);
    entity_mesh_set_material(sbox, entity, metal, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "barrel", 1.5f, 0.0f, 0.0f, barrel_mesh, &entity);
    entity_mesh_set_material(sbox, entity, barrel, 0);
    entity_mesh_set_material(sbox, entity, barrel_top, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "cactus", 2.0f, -0.5f, -4.5f, cactus_mesh, &entity);
    entity_mesh_set_material(sbox, entity, cactus, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "rock", 3.5f, -0.5f, -6.5f, rock_mesh, &entity);
    entity_mesh_set_material(sbox, entity, rock, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood panel", 3.5f, -0.5f, -9.0f, wood_panel_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood5, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood panel(2)", 2.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood5, 0);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood panel(3)", 5.0f, -0.5f, -8.2f, wood_panel_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood5, 0);
    glm_quat(entity->rotation, rad(-45.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame", 16.0f + 9.0f, -0.5f, -8.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame window", 16.0f + 9.0f, -0.5f, -5.0f,
        wood_frame_window_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(2)", 16.0f + 9.0f, -0.5f, -2.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(3)", 16.0f + 9.0f, -0.5f, 1.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(4)", 16.0f, -0.5f, -8.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(5)", 16.0f, -0.5f, -5.0f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame doorway", 16.0f, -0.5f, -2.0f,
        wood_frame_doorway_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame window", 16.0f, -0.5f, 1.0f,
        wood_frame_window_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 1.0f, 0.0f);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(7)", 17.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(8)", 20.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(9)", 23.5f, -0.5f, 2.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(-90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(10)", 17.5f, -0.5f, -9.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame doorway(2)", 20.5f, -0.5f, -9.5f,
        wood_frame_doorway_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "wood frame(11)", 23.5f, -0.5f, -9.5f, wood_frame_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entity_mesh_set_material(sbox, entity, wood5, 1);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    for (int x = 0; x < 3; x++) {
        for (int z = 0; z < 4; z++) {
            entity_init_mesh(sbox,
                "wood frame(ceiling)",
                19.0f + x * 3.0f, 2.5f, -8.0f + z * 3.0f,
                wood_frame_mesh, &entity);
            entity_mesh_set_material(sbox, entity, wood, 0);
            entity_mesh_set_material(sbox, entity, wood5, 1);
            glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
            glm_quat(entity->rotation, rad(90.0f), 0.0f, 0.0f, 1.0f);
            entlist_add(sbox, &map->entlist, entity);
        }
    }

    entity_init_mesh(sbox, "stone wall", 12.0f, -0.5f, -11.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "stone wall(2)", 12.0f, -0.5f, -9.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "stone wall(3)", 12.0f, -0.5f, -7.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "stone wall(4)", 12.0f, -0.5f, -5.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "stone wall(5)", 12.0f, -0.5f, -1.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "stone wall(6)", 12.0f, -0.5f, 1.0f, stone_wall_mesh, &entity);
    entity_mesh_set_material(sbox, entity, stone, 0);
    entlist_add(sbox, &map->entlist, entity);

    for (int i = 0; i < 12; i++) {
        if (i == 1 || i == 11) continue;
        entity_init_mesh(sbox,
            "stone wall",
            40.0f, -0.5f, -11.0f + i * 2.0f,
            stone_wall_mesh,
            &entity);
        entity_mesh_set_material(sbox, entity, stone, 0);
        entlist_add(sbox, &map->entlist, entity);
    }

    entity_init_mesh(sbox, "crate", 39.3f, 0.0f, -4.5f, crate_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate", 39.0f, 0.5f, -3.0f, crate2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate", 36.1f, 0.5f, 4.5f, crate2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "big crate", 36.1f, 2.5f, 4.5f, crate2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, crate2, 0);
    entlist_add(sbox, &map->entlist, entity);

    for (int i = 0; i < 5; i++) {
        entity_init_mesh(sbox,
            "chainlink fence",
            35.0f, -0.5f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(sbox, entity, chainlink, 0);
        entity_mesh_set_material(sbox, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);

        entity_init_mesh(sbox,
            "chainlink fence",
            35.0f, 1.5f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(sbox, entity, chainlink, 0);
        entity_mesh_set_material(sbox, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);

        entity_init_mesh(sbox,
            "chainlink fence",
            35.0f, 3.0f, -3.0f + i * 2.0f,
            chainlink_fence_mesh,
            &entity);
        entity_mesh_set_material(sbox, entity, chainlink, 0);
        entity_mesh_set_material(sbox, entity, wood, 1);
        glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);
    }

    for (int i = 0; i < 3; i++) {
        entity_init_mesh(sbox, "pipe", 36.0f, 1.5f + i * 4.0f, 11.0f, pipe_mesh, &entity);
        entity_mesh_set_material(sbox, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);

        entity_init_mesh(sbox, "pipe", 36.0f, 1.5f + i * 4.0f, 9.0f, pipe_mesh, &entity);
        entity_mesh_set_material(sbox, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);

        entity_init_mesh(sbox, "pipe", 36.0f, 1.5f + i * 4.0f, 7.0f, pipe_mesh, &entity);
        entity_mesh_set_material(sbox, entity, metal, 0);
        glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
        entlist_add(sbox, &map->entlist, entity);
    }

    entity_init_mesh(sbox, "chainlink fence", 0.0f, -0.5f, 0.8f, chainlink_fence_mesh, &entity);
    entity_mesh_set_material(sbox, entity, chainlink, 0);
    entity_mesh_set_material(sbox, entity, wood, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "container", -2.5f, -0.5f, -8.0f, container_mesh, &entity);
    entity_mesh_set_material(sbox, entity, container, 0);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe bend", 32.0f, 0.5f, 10.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe", 32.0f, 0.5f, 6.0f, pipe_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe(2)", 32.0f, 0.5f, 2.0f, pipe_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe bend(2)", 32.0f, 0.5f, -2.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 0.0f, 1.0f);
    glm_quat(entity->rotation, rad(180.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe bend(3)", 34.0f, 1.5f, 10.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe(4)", 34.0f, -1.5f, 11.0f, pipe_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    glm_quat(entity->rotation, rad(90.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe(5)", 34.0f, 1.5f, 6.0f, pipe_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe(6)", 34.0f, 1.5f, 2.0f, pipe_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "pipe bend(4)", 34.0f, 1.5f, -2.0f, pipe_bend_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    glm_quat(entity->rotation, rad(180.0f), 0.0f, 0.0f, 1.0f);
    glm_quat(entity->rotation, rad(180.0f), 1.0f, 0.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "table(2)", 9.0f, -0.5f, -7.0f, table2_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "bench(1)", 9.0f, -0.5f, -5.8f, bench_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "bench(2)", 9.0f, -0.5f, -8.2f, bench_mesh, &entity);
    entity_mesh_set_material(sbox, entity, wood, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "car", 9.0f, -0.5f, 0.0f, car_mesh, &entity);
    glm_quat(entity->rotation, rad(0.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entity_mesh_set_material(sbox, entity, wood, 1);
    entity_mesh_set_material(sbox, entity, metal, 2);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "helicopter", 8.0f, -0.5f, 16.0f, helicopter_mesh, &entity);
    glm_quat(entity->rotation, rad(45.0f), 0.0f, 1.0f, 0.0f);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entity_mesh_set_material(sbox, entity, wood, 1);
    entity->data.mesh.enable_collision = false;
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "water", 0.0f, -1.0f, 0.0f, water_mesh, &entity);
    entity_mesh_set_material(sbox, entity, water, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "underwater", 0.0f, -2.0f, 0.0f, underwater_mesh, &entity);
    entity_mesh_set_material(sbox, entity, sand, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "ship", 0.0f, -0.5f, -32.0f, ship_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "dock", 4.0f, -1.0f, -12.0f, dock_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "dock(2)", 4.0f, -1.0f, -18.0f, dock_mesh, &entity);
    entity_mesh_set_material(sbox, entity, concrete, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "vinyl #1", 2.0f, 0.4f, 3.25f, vinyl_mesh, &entity);
    entity_mesh_set_material(sbox, entity, water, 0);
    entity->data.mesh.is_pickup = true;
    entlist_add(sbox, &map->entlist, entity);

    vec3 sun_dir = {0.0f, -0.999f, 0.0f};
    vec3 sun_color = {6.0f, 5.5f, 5.0f};
    entity_init_sun_light(sbox, "sun", 0.0f, 0.0f, 0.0f, sun_dir, sun_color, &entity);
    entlist_add(sbox, &map->entlist, entity);

    /*vec3 color = {32.0f, 32.0f, 32.0f};
    entity_init_point_light(sbox, "point light", 0.0f, 1.5f, -2.5f, color, &entity);
    entlist_add(sbox, &map->entlist, entity);*/

    entity_init_mesh(sbox, "streetlight", 0.0f, -0.5f, -2.5f, streetlight_mesh, &entity);
    entity_mesh_set_material(sbox, entity, metal, 0);
    entity_mesh_set_material(sbox, entity, light, 1);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge", 15.5f, -0.5f, 6.0f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(2)", 15.5f, -0.5f, 10.0f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(3)", 24.5f, -0.5f, 6.0f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(4)", 24.5f, -0.5f, 10.0f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(5)", 23.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(6)", 21.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(7)", 19.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(8)", 17.0f, -0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(5)", 23.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(6)", 21.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(7)", 19.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(8)", 17.0f, 0.5f, 4.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(9)", 23.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(10)", 21.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(11)", 19.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    entity_init_mesh(sbox, "hedge(12)", 17.0f, -0.5f, 11.5f, hedge_mesh, &entity);
    entity_mesh_set_material(sbox, entity, leaves, 0);
    glm_quat(entity->rotation, rad(90.0f), 0.0f, 1.0f, 0.0f);
    entlist_add(sbox, &map->entlist, entity);

    for (int i = 0; i < 16; i++) {
        vec3 min = {15.0f + 1.0f, -0.5f, 4.5f + 1.0f};
        vec3 max = {24.5f - 1.0f, -0.5f, 11.5f - 1.0f};
        vec3 pos = {random(min[0], max[0]), random(min[1], max[1]), random(min[2], max[2])};
        mesh_t* mesh = (random(0.0f, 1.0f) > 0.5f) ? bush2_mesh : bush_mesh;

        entity_init_mesh(sbox, "bush", pos[0], pos[1], pos[2], mesh, &entity);
        entity_mesh_set_material(sbox, entity, leaves, 0);
        glm_quat(entity->rotation, rad(random(-180.0f, 180.0f)), 0.0f, 1.0f, 0.0f);
        glm_vec3_scale(entity->scale, random(1.0f, 3.0f), entity->scale);
        entity->data.mesh.enable_collision = false;
        entlist_add(sbox, &map->entlist, entity);
    }

    for (int i = 0; i < 10; i++) {
        float x = random(-32.0f, 32.0f);
        float z = random(-32.0f, 32.0f);

        vec3 size;
        bbox_get_size(&coin_mesh->bbox, size);

        vec3 start = {x, -0.5f + size[1], z};
        vec3 dir = {0.0f, -1.0f, 0.0f};
        float max_distance = 100.0f;
        trace_result_t trace;

        //r_add_line(sbox, &sbox->renderer, start, dir, COLOR_GREEN, 5.0f);

        //if (phys_line_trace(sbox, start, dir, max_distance, &sbox->map.entlist, -1, &trace)) {
        for (int j = -3; j <= 3; j++) {
            for (int k = -3; k <= 3; k++) {
                entity_init_mesh(sbox, "coin", x + j, start[1] + 0.1f, z + k, coin_mesh, &entity);
                entity_mesh_set_material(sbox, entity, coin, 0);
                entity->data.mesh.enable_collision = false;
                entlist_add(sbox, &map->entlist, entity);
            }
        }
            
        //}
    }

    const char* paths[6] = {
        "res/textures/skies/sky_right.png",
        "res/textures/skies/sky_left.png",
        "res/textures/skies/sky_top.png",
        "res/textures/skies/sky_bottom.png",
        "res/textures/skies/sky_front.png",
        "res/textures/skies/sky_back.png",
    };
    map->skybox = texture_load_cubemap(sbox, paths);

    info(sbox, "map loaded!");
    map->is_loaded = true;
    sbox->ui_state = UI_STATE_IN_GAME;
}

void map_free(sbox_t* sbox, map_t* map) {
    entlist_free(sbox, &map->entlist);
    map->is_loaded = false;
}

static void send_to_renderer(sbox_t* sbox, map_t* map) {
    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (!entity) continue;

        drawcall_t drawcall = {0};
        if (entity_get_drawcall(sbox, entity, &drawcall))
            r_add_drawcall(&sbox->renderer, drawcall);
    }
}

void map_tick(sbox_t* sbox, map_t* map) {
	if (!sbox->map.is_loaded) return;

	entlist_tick(sbox, &map->entlist);
    send_to_renderer(sbox, map);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        player_t* player = sbox->players[i];
        if (!player) continue;
        if (player->position[1] < -100.0f)
            player_add_damage(sbox, player, player->health);
    }
}
