#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

typedef struct sbox_t sbox_t;

typedef struct {
    char* input;
    char* history;
    size_t history_len;
} console_t;

void con_init(sbox_t* sbox, console_t* con);
void con_free(sbox_t* sbox, console_t* con);
void con_submit(sbox_t* sbox, console_t* con);
void con_add_history(sbox_t* sbox, console_t* con, const char* text);
void con_get_history(sbox_t* sbox, console_t* con, int lines, char** history);

#endif
