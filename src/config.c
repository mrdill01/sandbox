#include "config.h"
#include "quark.h"

void cvar_register(quark_t* quark, cvar_t* cvar, on_change_t on_change) {
	cvar->init = cvar->string;
    cvar->on_change = on_change;
    cvar->next = quark->cvars;
    quark->cvars = cvar;

    cvar_set(quark, cvar->name, cvar->string);
}

void cvar_set(quark_t* quark, const char* name, const char* value) {
    cvar_t* cvar = cvar_find(quark, name);
    if (!cvar) {
        error(quark, "cvar not found: %s", name);
		return;
	}

	if (cvar->is_cheat && !sv_cheats.value && strcmp(cvar->name, "sv_cheats") != 0) {
		error(quark, "changing the value of %s requires enabling sv_cheats", cvar->name);
		return;
	}
    
    cvar->string = value;
    cvar->value = atof(value);

    info(quark, "set %s to %s", name, cvar->string);

    if (cvar->on_change)
        cvar->on_change(quark);
}

cvar_t* cvar_get(quark_t* quark, const char* name) {
	cvar_t* cvar = cvar_find(quark, name);
    if (!cvar) {
        error(quark, "cvar not found: %s", name);
		return NULL;
	}

	return cvar;
}

void cvar_set_value(quark_t* quark, const char* name, float value) {
    char string[32];
    sprintf(string, "%f", value);
    cvar_set(quark, name, string);
}

void cvar_toggle(quark_t* quark, const char* name) {
	cvar_t* cvar = cvar_find(quark, name);
    if (!cvar) error(quark, "cvar not found: %s", name);
	if (cvar->value) cvar_set(quark, name, "0.0f");
	else cvar_set(quark, name, "1.0f");
}

cvar_t* cvar_find(quark_t* quark, const char* name) {
    cvar_t* cvar = quark->cvars;
    while (cvar) {
        if (strcmp(cvar->name, name) == 0)
            return cvar;
        cvar = cvar->next;
    }

    return NULL;
}

void cfg_write(quark_t* quark, const char* path) {
	info(quark, "writing config to %s", path);
	clear_file(quark, path);

	FILE* fp = fopen(path, "a");
	if (!fp) {
		error(quark, "failed to open %s for writing", path);
		return;
	}

	cvar_t* cvar = quark->cvars;
	while (cvar) {
		fprintf(fp, "%s %s\n", cvar->name, cvar->string);
		cvar = cvar->next;
	}

	fclose(fp);
}

void cfg_read(quark_t* quark, const char* path) {
	info(quark, "reading config from %s", path);

	char* text = load_file(quark, path);
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
			error(quark, "in %s: no value for key %s", path, name);
			continue;
		}

		first = false;
		cvar_set(quark, name, value);
	}

	free(text);
}
