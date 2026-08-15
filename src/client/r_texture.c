#include "render.h"
#include "../shared/quark.h"

#include "../../include/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"

static uint32_t get_internal_format(quark_t* quark, texture_format_t format) {
    switch (format) {
    case TEX_FORMAT_RGB: return GL_RGB;
    case TEX_FORMAT_RGBA: return GL_RGBA;
    case TEX_FORMAT_RGBA_F16: return GL_RGBA16F;
    case TEX_FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
    default: unreachable(quark);
    }

    return 0;
}

static uint32_t get_gl_format(quark_t* quark, texture_format_t format) {
    switch (format) {
    case TEX_FORMAT_RGB: return GL_RGB;
    case TEX_FORMAT_RGBA: return GL_RGBA;
    case TEX_FORMAT_RGBA_F16: return GL_RGBA;
    case TEX_FORMAT_DEPTH: return GL_DEPTH_COMPONENT;
    default: unreachable(quark);
    }

    return 0;
}

texture_t* texture_new(quark_t* quark, int width, int height, uint8_t* data,
    texture_format_t format, texture_filter_t filter)
{
    uint32_t id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    uint32_t gl_internal_format = get_internal_format(quark, format);
    uint32_t gl_format = get_gl_format(quark, format);
    uint32_t gl_type = (format == TEX_FORMAT_RGBA_F16 ||
        format == TEX_FORMAT_DEPTH) ? GL_FLOAT : GL_UNSIGNED_BYTE;
    
    glTexImage2D(GL_TEXTURE_2D,
        0,
        gl_internal_format,
        width,
        height,
        0,
        gl_format,
        gl_type,
        data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLint gl_filter;
    switch (filter) {
    case TEX_FILTER_NEAREST: gl_filter = GL_NEAREST; break;
    case TEX_FILTER_LINEAR: gl_filter = GL_LINEAR; break;
    default: unreachable(quark);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);

    glGenerateMipmap(GL_TEXTURE_2D);

    texture_t* texture = malloc(sizeof(texture_t));
    texture->id = id;
    texture->type = TEX_2D;
    texture->width = width;
    texture->height = height;
    texture->format = format;
    texture->filter = filter;

    texture->next = quark->textures;
    quark->textures = texture;
    return texture;
}

texture_t* texture_load(quark_t* quark, const char* path, texture_filter_t filter) {
    info(quark, "loading texture %s", path);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        error(quark, "failed to load texture %s", path);
        return NULL;
    }

    texture_t* texture = texture_new(quark, width, height, data, TEX_FORMAT_RGBA, filter);
    stbi_image_free(data);
    return texture;
}

texture_t* texture_load_cubemap(quark_t* quark, const char* paths[6]) {
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, channels;
    unsigned char* data;
    for (int i = 0; i < 6; i++) {
        const char* path = paths[i];
        data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
        if (!data) {
            error(quark, "failed to load texture %s", path);
            return NULL;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    texture_t* texture = malloc(sizeof(texture_t));
    texture->id = id;
    texture->type = TEX_CUBE;
    texture->width = width;
    texture->height = height;

    texture->next = quark->textures;
    quark->textures = texture;
    return texture;
}

void texture_free(quark_t* quark, texture_t* texture) {
    if (!texture) return;
    glDeleteTextures(1, &texture->id);
    free(texture);
}
