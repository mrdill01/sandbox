#ifndef CONSOLE_H
#define CONSOLE_H

#include "config.h"
#include "command.h"

#include <stddef.h>

#define CON_MAX_INPUT 256
#define CON_MAX_HISTORY 8192
#define CON_MAX_ARGS 16
#define CON_LINES_PER_PAGE 22

typedef struct sbox_t sbox_t;

typedef struct {
    char input[CON_MAX_INPUT];
    char* history[CON_MAX_HISTORY];
    size_t history_len;
    int scroll;
} console_t;

void con_init(sbox_t* sbox, console_t* con);
void con_free(sbox_t* sbox, console_t* con);

void con_open(sbox_t* sbox, console_t* con);
void con_close(sbox_t* sbox, console_t* con);

void con_submit(sbox_t* sbox, console_t* con);
void con_add_history(sbox_t* sbox, console_t* con, const char* text);

#endif
