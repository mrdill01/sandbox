#include "player.h"
#include "quark.h"

void bot_init(quark_t* quark, player_t* bot) {
    bot->bot.last_speech = quark->time;
    bot->bot.last_speech_word = quark->time;
}

static void move_to_target(quark_t* quark, player_t* bot, vec3 target, float stop_distance) {
    if (!target)
        return;
    
    float distance = glm_vec3_distance(bot->position, target);
    if (distance < stop_distance)
        return;

    vec3 direction;
    glm_vec3_sub(target, bot->position, direction);
    glm_vec3_copy(direction, bot->target_dir);
    glm_vec3_normalize(bot->target_dir);
}

static void speak(quark_t* quark, player_t* bot, int nsounds) {
    if (bot->bot.num_speech_words > 0) {
        if (bot->bot.last_speech_word)
        bot->bot.num_speech_words--;

        sound_t* sound = quark->audio.speech_sounds[(int)random(0, NUM_SPEECH_SOUNDS)];
        a_play(quark, &quark->audio, sound, bot->position, random(0.85f, 1.15f));
        bot->bot.last_speech_word = quark->time;
    }

    if (quark->time - bot->bot.last_speech < 3.0f)
        return;
    bot->bot.last_speech = quark->time;
    bot->bot.num_speech_words = 5;
}

void bot_tick(quark_t* quark, player_t* bot) {
    glm_vec3_zero(bot->target_dir);
    bot->buttons = 0;

    if ((int)random(0, 150) == 0)
        bot->buttons |= PLAYER_BUTTON_JUMP;

    move_to_target(quark, bot, quark->player->position, 4.0f);
    speak(quark, bot, 5);
}
