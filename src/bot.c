#include "player.h"
#include "sbox.h"

static void move_to_target(sbox_t* sbox, player_t* player, vec3 target, float stop_distance) {
    if (!target)
        return;
    
    float distance = glm_vec3_distance(player->position, target);
    if (distance < stop_distance)
        return;

    vec3 direction;
    glm_vec3_sub(target, player->position, direction);
    glm_vec3_copy(direction, player->target_dir);
    glm_vec3_normalize(player->target_dir);
}

static void speak(sbox_t* sbox, player_t* player, int nsounds) {
    if (sbox->time - player->bot.last_speech < 5.0f)
        return;
    player->bot.last_speech = sbox->time;

    sound_t* sound = sbox->audio.speech_sounds[(int)random(0, NUM_SPEECH_SOUNDS)];
    a_play(sbox, &sbox->audio, sound, player->position, random(0.85f, 1.15f));
}

void bot_tick(sbox_t* sbox, player_t* player) {
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;

    if ((int)random(0, 150) == 0)
        player->buttons |= PLAYER_BUTTON_JUMP;

    move_to_target(sbox, player, sbox->player->position, 4.0f);
    speak(sbox, player, 5);
}
