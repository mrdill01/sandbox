#ifndef AUDIO_H
#define AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>

typedef struct sbox_t sbox_t;
typedef struct player_t player_t;
typedef struct camera_t camera_t;

typedef struct {
    ALCdevice* device;
    ALCcontext* context;
    ALenum last_error;
} audio_t;

void a_init(sbox_t* sbox, audio_t* audio);
void a_free(sbox_t* sbox, audio_t* audio);
void a_tick(sbox_t* sbox, audio_t* audio, player_t* player, camera_t* camera);

#endif
