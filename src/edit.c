#include "edit.h"
#include "sbox.h"
#include "player.h"

void edit_init(sbox_t* sbox, editor_t* editor) {
    editor->selection = NULL;
}

void edit_tick(sbox_t* sbox, editor_t* editor, player_t* player) {
    if (!edit_mode.value) return;

    if (editor->selection) {
        vec3 half_size;
        bbox_get_half_size(&player->editor.selection->local_bbox, half_size);

        vec3 center;
        bbox_get_center(&player->editor.selection->world_bbox, center);

        glm_vec3_copy(editor->trace.point, player->editor.selection->position);

        r_add_line_box(sbox, &sbox->renderer,
            &player->editor.selection->world_bbox, COLOR_LIGHT_BLUE, 0.0f);
    }
}
