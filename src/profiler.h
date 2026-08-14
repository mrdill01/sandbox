#ifndef PROFILER_H
#define PROFILER_H

#define MAX_PROFILER_FUNCS 128

typedef struct quark_t quark_t;

typedef struct {
    char* name;
    double start;
    double time;
    double percentage;
} profiler_entry_t;

typedef struct {
    profiler_entry_t entries[MAX_PROFILER_FUNCS];
    double total;
} profiler_t;

void prof_init(quark_t* quark, profiler_t* prof);
void prof_free(quark_t* quark, profiler_t* prof);
void prof_tick(quark_t* quark, profiler_t* prof);

void prof_start_impl(quark_t* quark, profiler_t* prof, const char* name);
void prof_end_impl(quark_t* quark, profiler_t* prof, const char* name);

#define prof_start(quark, prof) prof_start_impl(quark, prof, __func__);
#define prof_end(quark, prof) prof_end_impl(quark, prof, __func__);

#endif
