#ifndef PROFILER_H
#define PROFILER_H

#define MAX_PROFILER_FUNCS 128

typedef struct sbox_t sbox_t;

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

void prof_init(sbox_t* sbox, profiler_t* prof);
void prof_free(sbox_t* sbox, profiler_t* prof);
void prof_tick(sbox_t* sbox, profiler_t* prof);

void prof_start_impl(sbox_t* sbox, profiler_t* prof, const char* name);
void prof_end_impl(sbox_t* sbox, profiler_t* prof, const char* name);

#define prof_start(sbox, prof) prof_start_impl(sbox, prof, __func__);
#define prof_end(sbox, prof) prof_end_impl(sbox, prof, __func__);

#endif
