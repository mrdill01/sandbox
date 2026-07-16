#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

static int64_t compile_shader(sbox_t* sbox, const char* src, const char* name, int type) {
    info(sbox, "compiling shader %s", name);

    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetShaderInfoLog(id, 512, NULL, buffer);
        error(sbox, "failed to compile shader %s: %s", name, buffer);
        return -1;
    }

    return id;
}

shader_t* shader_new(sbox_t* sbox, const char* vs, const char* vname, const char* fs, const char* fname) {
    uint32_t vertex_shader = compile_shader(sbox, vs, vname, GL_VERTEX_SHADER);
    uint32_t fragment_shader = compile_shader(sbox, fs, fname, GL_FRAGMENT_SHADER);

    uint32_t id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetProgramInfoLog(id, 512, NULL, buffer);
        error(sbox, "failed to link shader: %s", buffer);
        return NULL;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    shader_t* shader = malloc(sizeof(shader_t));
    shader->id = id;
    shader->vs_path = vname;
    shader->fs_path = fname;

    shader->next = sbox->shaders;
    sbox->shaders = shader;
    return shader;
}

shader_t* shader_load(sbox_t* sbox, const char* vpath, const char* fpath) {
    info(sbox, "loading shaders %s and %s", vpath, fpath);
    const char* vs = load_file(sbox, vpath);
    const char* fs = load_file(sbox, fpath);
    return shader_new(sbox, vs, vpath, fs, fpath);
}

void shader_free(sbox_t* sbox, shader_t* shader) {
    if (!shader) return;
    glDeleteProgram(shader->id);
    free(shader);
}
