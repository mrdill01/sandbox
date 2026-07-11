#include "render.h"
#include "sbox.h"

material_t* material_load(sbox_t* sbox,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent,
    int phys_mat)
{
    material_t* material = malloc(sizeof(material_t));
    material->albedo = texture_load(sbox, albedo_path);
    material->roughness = texture_load(sbox, roughness_path);
    material->normal = texture_load(sbox, normal_path);
    material->tilex = tilex;
    material->tiley = tiley;
    material->is_translucent = is_translucent;
    material->is_water = false;
    material->phys_mat = phys_mat;
    
    material->next = sbox->materials;
    sbox->materials = material;
    return material;
}

void material_free(sbox_t* sbox, material_t* material) {
    if (!material) return;
    free(material);
}
