#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define DEFAULT_CFG_PATH "settings.txt"

typedef struct quark_t quark_t;

typedef void (*on_change_t)(quark_t*);

typedef struct cvar_t {
    const char* name;
    const char* string;
    bool save;
    bool is_cheat;
    const char* desc;
    float value;
    const char* init;
    on_change_t on_change;
    struct cvar_t* next;
} cvar_t;

void cvar_register(quark_t* quark, cvar_t* cvar, on_change_t on_change);
void cvar_set(quark_t* quark, const char* name, const char* value);
cvar_t* cvar_get(quark_t* quark, const char* name);
void cvar_set_value(quark_t* quark, const char* name, float value);
void cvar_toggle(quark_t* quark, const char* name);
cvar_t* cvar_find(quark_t* quark, const char* name);

void cfg_write(quark_t* quark, const char* path);
void cfg_read(quark_t* quark, const char* path);

#endif
