#include "console.h"
#include "sbox.h"

void con_init(sbox_t* sbox, console_t* con) {
    con->input = NULL;
    con->history = NULL;
    con->history_len = 0;
}

void con_free(sbox_t* sbox, console_t* con) {
    free(con->input);
    free(con->history);
}

void con_submit(sbox_t* sbox, console_t* con) {

}

void con_add_history(sbox_t* sbox, console_t* con, const char* text) {
    con->history_len += strlen(text);
    con->history = realloc(con->history, con->history_len + 1);
    strcat(con->history, text);
}
