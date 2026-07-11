#include "audio.h"
#include "sbox.h"
#include "player.h"
#include "render.h"

static void list_audio_devices(sbox_t* sbox, const ALCchar *devices) {
    const ALCchar* device = devices;
    const ALCchar* next = devices + 1;
    size_t len = 0;
    int num = 1;

    info(sbox, "available audio devices:");
    while (device && *device != '\0' && next && *next != '\0') {
        info(sbox, "  %d. %s", num, device);
        len = strlen(device);
        device += (len + 1);
        next += (len + 2);
        num++;
    }
}

void a_init(sbox_t* sbox, audio_t* audio) {
    info(sbox, "setting up audio...");

    alGetError();
    
    ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        error(sbox, "enumerating audio devices is not supported");

    list_audio_devices(sbox, alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    ALCenum err;

    audio->device = alcOpenDevice("OpenAL Soft");
    if (!audio->device ||  ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(sbox, "failed to open audio device: %d", err);
        return;
    }

    info(sbox, "opened audio device...");

    audio->context = alcCreateContext(audio->device, NULL);
    if (!audio->context || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(sbox, "failed to create OpenAL context: %d", err);
        return;
    }

    info(sbox, "created audio context...");

    if (!alcMakeContextCurrent(audio->context) || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(sbox, "failed to make OpenAL context current: %d", err);
        return;
    }

    info(sbox, "made audio context current...");
    info(sbox, "audio initialized!");
}

void a_free(sbox_t* sbox, audio_t* audio) {
    info(sbox, "shutting down audio...");

    info(sbox, "audio shut down!");
}

void a_tick(sbox_t* sbox, audio_t* audio, player_t* player, camera_t* camera) {
    ALfloat orientation[] = {
        camera->forward[0], camera->forward[1], camera->forward[2],
        camera->up[0], camera->up[1], camera->up[2],
    };
    ALenum err;

    alListener3f(AL_POSITION, player->position[0], player->position[1], player->position[2]);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_POSITION: %d", err);
    
    alListener3f(AL_VELOCITY, player->velocity[0], player->velocity[1], player->velocity[2]);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_VELOCITY: %d", err);
    
    alListenerfv(AL_ORIENTATION, orientation);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_ORIENTATION: %d", err);
}
