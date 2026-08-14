#include "render.h"
#include "quark.h"

#include <string.h>

material_t* material_load(quark_t* quark,
    const char* name,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent,
    int phys_mat)
{
    material_t* material = malloc(sizeof(material_t));
    size_t len = strlen(name);
    material->name = malloc(len + 1);
    strcpy(material->name, name);
    material->name[len] = '\0';

    material->albedo = texture_load(quark, albedo_path, TEX_FILTER_NEAREST);
    material->roughness = texture_load(quark, roughness_path, TEX_FILTER_NEAREST);
    material->normal = texture_load(quark, normal_path, TEX_FILTER_NEAREST);
    material->wind_factor = 0.0f;
    material->tilex = tilex;
    material->tiley = tiley;
    material->scrollx = 0.0f;
    material->scrolly = 0.0f;
    material->scroll_speed = 0.0f;
    material->is_translucent = is_translucent;
    material->is_water = false;
    material->phys_mat = phys_mat;
    
    material->next = quark->materials;
    quark->materials = material;
    return material;
}

void material_free(quark_t* quark, material_t* material) {
    if (!material) return;
    free(material->name);
    free(material);
}
