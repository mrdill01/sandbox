#ifndef AUDIO_H
#define AUDIO_H

#include "physics.h"

#include <AL/al.h>
#include <AL/alc.h>

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

    sound_t* jump_sound;
    sound_t* jump_land_sounds[PHYSMAT_MAX];
    sound_t* step_sounds[PHYSMAT_MAX];
    sound_t* enter_water_sound;
} audio_t;

void a_init(sbox_t* sbox, audio_t* audio);
void a_free(sbox_t* sbox, audio_t* audio);
void a_tick(sbox_t* sbox, audio_t* audio, player_t* player, camera_t* camera);
void a_play(sbox_t* sbox, audio_t* audio, sound_t* sound, float pitch);

sound_t* sound_load(sbox_t* sbox, audio_t* audio, const char* path);
void sound_free(sbox_t* sbox, audio_t* audio, sound_t* sound);

#endif
