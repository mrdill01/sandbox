#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

texture_t* texture_new(sbox_t* sbox, int width, int height, uint8_t* data) {
    uint32_t id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    texture_t* texture = malloc(sizeof(texture_t));
    texture->id = id;
    texture->width = width;
    texture->height = height;

    texture->next = sbox->textures;
    sbox->textures = texture;
    return texture;
}

texture_t* texture_load(sbox_t* sbox, const char* path) {
    info(sbox, "loading texture %s", path);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        error(sbox, "failed to load texture '%s'", path);
        return NULL;
    }

    texture_t* texture = texture_new(sbox, width, height, data);
    stbi_image_free(data);
    return texture;
}

void texture_free(sbox_t* sbox, texture_t* texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
}
