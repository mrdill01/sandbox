#ifndef COMMAND_H
#define COMMAND_H

typedef struct sbox_t sbox_t;

typedef struct cmd_t {
    const char* name;
    const char* usage;
    const char* desc;
    struct cmd_t* next;
} cmd_t;

extern cmd_t help;
extern cmd_t cmdlist;
extern cmd_t cvarlist;
extern cmd_t reset;
extern cmd_t clear;
extern cmd_t host;
extern cmd_t connect_;
extern cmd_t disconnect;
extern cmd_t quit;

void cmd_init(sbox_t* sbox);
void cmd_register(sbox_t* sbox, cmd_t* cmd);
cmd_t* cmd_find(sbox_t* sbox, const char* name);
void cmd_run(sbox_t* sbox, const char* name, const char** args, int argc);

#endif
