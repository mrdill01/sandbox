#ifndef AUDIO_H
#define AUDIO_H

#include "physics.h"

#include <stdbool.h>

#include <AL/al.h>
#include <AL/alc.h>

#define NUM_SPEECH_SOUNDS 19
#define NUM_HOTBAR_SELECT_SOUNDS 4

typedef struct sbox_t sbox_t;
typedef struct player_t player_t;
typedef struct camera_t camera_t;

typedef struct sound_t {
    ALuint buffer;
    ALuint source;
    struct sound_t* next;
} sound_t;

typedef struct {
    ALCdevice* device;
    ALCcontext* context;
    sound_t* sounds;

    int sounds_playing;

    sound_t* jump_sound;
    sound_t* jump_land_base_sound;
    sound_t* jump_land_sounds[NUM_PHYS_MAT];
    sound_t* step_sounds[NUM_PHYS_MAT];
    sound_t* bullet_hit_sounds[NUM_PHYS_MAT];
    sound_t* speech_sounds[NUM_SPEECH_SOUNDS];
    sound_t* enter_water_sound;
    sound_t* exit_water_sound;
    sound_t* explosion_sound;
    sound_t* pickup_coin_sound;
    sound_t* hurt_sound;
    sound_t* fall_damage_sound;
    sound_t* inventory_open_sound;
    sound_t* inventory_close_sound;
    sound_t* hotbar_select_sounds[NUM_HOTBAR_SELECT_SOUNDS];
    sound_t* button_hover_sound;
    sound_t* button_press_sound;
} audio_t;

void a_init(sbox_t* sbox, audio_t* audio);
void a_free(sbox_t* sbox, audio_t* audio);
void a_tick(sbox_t* sbox, audio_t* audio, player_t* player, camera_t* camera);
void a_play(sbox_t* sbox, audio_t* audio, sound_t* sound, vec3 position, float pitch);
int a_get_max_source_count(sbox_t* sbox, audio_t* audio);

sound_t* sound_load(sbox_t* sbox, audio_t* audio, const char* path);
void sound_free(sbox_t* sbox, audio_t* audio, sound_t* sound);
bool sound_is_playing(sbox_t* sbox, sound_t* sound);

#endif
