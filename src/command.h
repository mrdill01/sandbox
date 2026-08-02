#ifndef COMMAND_H
#define COMMAND_H

typedef struct sbox_t sbox_t;

typedef struct cmd_t {
    const char* name;
    const char* desc;
    struct cmd_t* next;
} cmd_t;

extern cmd_t host;
extern cmd_t disconnect;
extern cmd_t quit;

void cmd_init(sbox_t* sbox);
void cmd_register(sbox_t* sbox, cmd_t* cmd);
void cmd_run(sbox_t* sbox, const char* name, const char** args, int argc);

#endif
