#include "config.h"
#include "sbox.h"

void cvar_register(sbox_t* sbox, cvar_t* cvar, on_change_t on_change) {
    cvar->on_change = on_change;
    cvar->next = sbox->cvars;
    sbox->cvars = cvar;

    cvar_set(sbox, cvar->name, cvar->string);
}

void cvar_set(sbox_t* sbox, const char* name, const char* value) {
    cvar_t* cvar = cvar_find(sbox, name);
    if (!cvar)
        error(sbox, "cvar not found: %s", name);
    
    cvar->string = value;
    cvar->value = atof(value);

    info(sbox, "set %s to %g", name, cvar->value);

    if (cvar->on_change)
        cvar->on_change(sbox);
}

void cvar_set_value(sbox_t* sbox, const char* name, float value) {
    char string[32];
    sprintf(string, "%f", value);
    cvar_set(sbox, name, string);
}

cvar_t* cvar_find(sbox_t* sbox, const char* name) {
    cvar_t* cvar = sbox->cvars;
    while (cvar) {
        if (strcmp(cvar->name, name) == 0)
            return cvar;
        cvar = cvar->next;
    }

    return NULL;
}
