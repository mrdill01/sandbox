#include "profiler.h"
#include "quark.h"

void prof_init(quark_t* quark, profiler_t* prof) {
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        entry->name = NULL;
        entry->start = 0.0f;
        entry->time = 0.0f;
    }
}

void prof_free(quark_t* quark, profiler_t* prof) {
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        if (!entry->name) continue;
        free(entry->name);
    }
}

void prof_tick(quark_t* quark, profiler_t* prof) {
    prof->total = 0.0;
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        if (!entry->name) continue;
        prof->total += entry->time;
    }

    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        if (!entry->name) continue;
        entry->percentage = entry->time / prof->total;
    }
}

void prof_start_impl(quark_t* quark, profiler_t* prof, const char* name) {
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];

        if (!entry->name) {
            entry->name = malloc(strlen(name) + 1);
            strcpy(entry->name, name);
            entry->start = (double)clock() / CLOCKS_PER_SEC;
            return;
        }

        if (strcmp(entry->name, name) == 0) {
            entry->start = (double)clock() / CLOCKS_PER_SEC;
            return;
        }
    }
}

void prof_end_impl(quark_t* quark, profiler_t* prof, const char* name) {
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        if (!entry->name) continue;
        if (strcmp(entry->name, name) == 0) {
            entry->time = ((double)clock() / CLOCKS_PER_SEC) - entry->start;
            return;
        }
    }
}
