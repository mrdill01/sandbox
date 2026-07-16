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

void cvar_toggle(sbox_t* sbox, const char* name) {
	cvar_t* cvar = cvar_find(sbox, name);
    if (!cvar) error(sbox, "cvar not found: %s", name);
	if (cvar->value) cvar_set(sbox, name, "0.0f");
	else cvar_set(sbox, name, "1.0f");
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

void cfg_write(sbox_t* sbox, const char* path) {
	info(sbox, "writing config to %s", path);
	clear_file(sbox, path);

	FILE* fp = fopen(path, "a");
	if (!fp) {
		error(sbox, "failed to open %s for writing", path);
		return;
	}

	cvar_t* cvar = sbox->cvars;
	while (cvar) {
		fprintf(fp, "%s %s\n", cvar->name, cvar->string);
		cvar = cvar->next;
	}

	fclose(fp);
}

void cfg_read(sbox_t* sbox, const char* path) {
	info(sbox, "reading config from %s", path);

	char* text = load_file(sbox, path);
	const char* delim = " \r\n";

	const char* name;
	const char* value;
	bool first = true;

	for (;;) {
		name = strtok((first) ? text : NULL, delim);
		if (!name)
			break;

		value = strtok(NULL, delim);
		if (!value) {
			error(sbox, "in %s: no value for key %s", path, name);
			continue;
		}

		first = false;
		cvar_set(sbox, name, value);
	}

	free(text);
}
