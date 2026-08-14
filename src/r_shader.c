#include "render.h"
#include "quark.h"

#include "../include/gl.h"

static int64_t compile_shader(quark_t* quark, const char* src, const char* name, int type) {
    info(quark, "compiling shader %s", name);

    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetShaderInfoLog(id, 512, NULL, buffer);
        error(quark, "failed to compile shader %s: %s", name, buffer);
        return -1;
    }

    return id;
}

shader_t* shader_new(quark_t* quark,
    const char* name,
    const char* vs, const char* vname,
    const char* fs, const char* fname) {
    uint32_t vertex_shader = compile_shader(quark, vs, vname, GL_VERTEX_SHADER);
    uint32_t fragment_shader = compile_shader(quark, fs, fname, GL_FRAGMENT_SHADER);

    uint32_t id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetProgramInfoLog(id, 512, NULL, buffer);
        error(quark, "failed to link shader: %s", buffer);
        return NULL;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    shader_t* shader = malloc(sizeof(shader_t));
    size_t len = strlen(name);
    shader->name = malloc(len + 1);
    strcpy(shader->name, name);
    shader->name[len] = '\0';

    shader->id = id;
    shader->vs_path = vname;
    shader->fs_path = fname;

    shader->next = quark->shaders;
    quark->shaders = shader;
    return shader;
}

shader_t* shader_load(quark_t* quark, const char* name, const char* vpath, const char* fpath) {
    info(quark, "loading shader %s (%s + %s)", name, vpath, fpath);
    const char* vs = load_file(quark, vpath);
    const char* fs = load_file(quark, fpath);
    return shader_new(quark, name, vs, vpath, fs, fpath);
}

void shader_free(quark_t* quark, shader_t* shader) {
    if (!shader) return;
    free(shader->name);
    glDeleteProgram(shader->id);
    free(shader);
}
