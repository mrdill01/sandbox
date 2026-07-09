#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct sbox_t sbox_t;

typedef void (*on_change_t)(sbox_t*);

typedef struct cvar_t {
    const char* name;
    const char* string;
    bool save;
    float value;
    on_change_t on_change;
    struct cvar_t* next;
} cvar_t;

void cvar_register(sbox_t* sbox, cvar_t* cvar, on_change_t on_change);
void cvar_set(sbox_t* sbox, const char* name, const char* value);
void cvar_set_value(sbox_t* sbox, const char* name, float value);
cvar_t* cvar_find(sbox_t* sbox, const char* name);

#endif
