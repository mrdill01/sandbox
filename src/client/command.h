#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

typedef struct quark_t quark_t;

typedef struct cmd_t {
    const char* name;
    const char* usage;
    const char* desc;
    bool is_cheat;
    struct cmd_t* next;
} cmd_t;

extern cmd_t help;
extern cmd_t cmdlist;
extern cmd_t cvarlist;
extern cmd_t reset;
extern cmd_t clear;
extern cmd_t teleport;
extern cmd_t bot;
extern cmd_t host;
extern cmd_t connect_;
extern cmd_t disconnect;
extern cmd_t quit;

void cmd_init(quark_t* quark);
void cmd_register(quark_t* quark, cmd_t* cmd);
cmd_t* cmd_find(quark_t* quark, const char* name);
void cmd_run(quark_t* quark, const char* name, const char** args, int argc);
void cmd_show_usage(quark_t* quark, const char* name);

#endif
