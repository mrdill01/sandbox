#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

#define MAX_ATTACHMENTS 4

framebuffer_t* framebuffer_new(sbox_t* sbox) {
    framebuffer_t* framebuffer = malloc(sizeof(framebuffer_t));
    glGenFramebuffers(1, &framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    framebuffer->ntextures = 0;
    framebuffer->textures = NULL;
    framebuffer->depth_buffer = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return framebuffer;
}

void framebuffer_add_texture(sbox_t* sbox, framebuffer_t* framebuffer, int width, int height) {
    if (framebuffer->ntextures >= MAX_ATTACHMENTS) {
        error(sbox, "too many framebuffer attachments (max is %d)", MAX_ATTACHMENTS);
        return;
    }

    framebuffer->textures = realloc(framebuffer->textures,
        sizeof(texture_t*) * (framebuffer->ntextures + 1));

    texture_t* texture = texture_new(sbox, width, height, NULL);
    framebuffer->textures[framebuffer->ntextures++] = texture;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        width,
        height,
        0,
        GL_RGBA,
        GL_FLOAT,
        NULL);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + (framebuffer->ntextures - 1),
        GL_TEXTURE_2D,
        texture->id,
        0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_add_depth_buffer(sbox_t* sbox, framebuffer_t* framebuffer, int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glGenRenderbuffers(1, &framebuffer->depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        framebuffer->depth_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_finish(sbox_t* sbox, framebuffer_t* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    uint32_t* attachments = malloc(sizeof(uint32_t) * framebuffer->ntextures);
    for (size_t i = 0; i < framebuffer->ntextures; i++)
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(framebuffer->ntextures, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        error(sbox, "framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    free(attachments);
}

void framebuffer_free(framebuffer_t* framebuffer) {
    if (!framebuffer) return;
    glDeleteFramebuffers(1, &framebuffer->id);
}
