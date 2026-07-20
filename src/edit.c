#include "edit.h"
#include "sbox.h"

void edit_init(sbox_t* sbox, editor_t* editor) {
    editor->selection = NULL;
}

static void update_selection_position(sbox_t* sbox, editor_t* editor) {
}

void edit_tick(sbox_t* sbox, editor_t* editor) {
    if (editor->selection) {

        glm_vec3_copy(editor->selection_position, editor->selection->position);
    }
}
