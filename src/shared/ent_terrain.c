#include "entity.h"
#include "quark.h"

float entity_terrain_get_height(quark_t* quark, entity_t* entity, float u, float v) {
    if (entity->type != ENTITY_TERRAIN || !entity->data.terrain.heightmap) return 0.0f;
    entity_terrain_t* terrain = &entity->data.terrain;

    //return random(0.0f, 1.0f) * entity->scale[1];
    size_t index = ((v * terrain->heightmap->width) + u) * 4;
    return terrain->heightmap->data[index] / 255.0f;
}

static void create_mesh(quark_t* quark, entity_t* entity, entity_terrain_t* terrain) {
    int n = 40;
    mesh_buffer_t** buffers = malloc(sizeof(mesh_buffer_t*));
    mesh_buffer_t* buffer = mesh_buffer_new(quark, 0, 0);
    buffers[0] = buffer;

    bbox_t bbox = {0};

    for (int j = 0; j <= n; j++) {
        for (int i = 0; i <= n; i++) {
            float x = (float)j / n;
            float z = (float)i / n;
            float y = entity_terrain_get_height(quark, entity, x, z);

            buffer->vertices = realloc(buffer->vertices, (buffer->nvertices + 9) * sizeof(float));
            buffer->vertices[buffer->nvertices++] = x;
            buffer->vertices[buffer->nvertices++] = y;
            buffer->vertices[buffer->nvertices++] = z;

            buffer->vertices[buffer->nvertices++] = 0.0f;
            buffer->vertices[buffer->nvertices++] = 1.0f;
            buffer->vertices[buffer->nvertices++] = 0.0f;

            buffer->vertices[buffer->nvertices++] = ((float)j / n) * n;
            buffer->vertices[buffer->nvertices++] = ((float)i / n) * n;

            buffer->vertices[buffer->nvertices++] = 0.0f;

            bbox.min[0] = min(bbox.min[0], x);
            bbox.min[1] = min(bbox.min[1], y);
            bbox.min[2] = min(bbox.min[2], z);

            bbox.max[0] = max(bbox.max[0], x);
            bbox.max[1] = max(bbox.max[1], y);
            bbox.max[2] = max(bbox.max[2], z);
        }
    }

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            int row1 = j * (n + 1);
            int row2 = (j + 1) * (n + 1);

            buffer->indices = realloc(buffer->indices, (buffer->nindices + 6) * sizeof(uint32_t));
            buffer->indices[buffer->nindices++] = row1 + i;
            buffer->indices[buffer->nindices++] = row1 + i + 1;
            buffer->indices[buffer->nindices++] = row2 + i + 1;

            buffer->indices[buffer->nindices++] = row1 + i;
            buffer->indices[buffer->nindices++] = row2 + i + 1;
            buffer->indices[buffer->nindices++] = row2 + i;
        }
    }

    if (terrain->mesh)
        mesh_free(quark, terrain->mesh);
    terrain->mesh = mesh_new(quark, buffers, 1, 1, bbox);
}

void entity_init_terrain(quark_t* quark,
    const char* name, vec3 position, vec3 scale, texture_t* heightmap, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_TERRAIN, position, &entity);
    glm_vec3_copy(scale, entity->scale);
    entity->data.terrain.heightmap = heightmap;
    entity->data.terrain.mesh = NULL;
    create_mesh(quark, entity, &entity->data.terrain);
    for (int i = 0; i < MAX_MATERIALS; i++)
        entity->data.terrain.materials[i] = NULL;

    *out = entity;
}

void entity_terrain_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot) {
    if (!entity || entity->type != ENTITY_TERRAIN) return;
	
	if (slot >= MAX_MATERIALS) {
		error(quark, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.terrain.materials[slot] = material;
}
