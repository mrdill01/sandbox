#include "audio.h"
#include "sbox.h"
#include "player.h"
#include "render.h"

#include <SDL2/SDL_audio.h>

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

    audio->device = alcOpenDevice(NULL);
    if (!audio->device || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
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

    audio->jump_sound = sound_load(sbox, audio, "res/sounds/jump.wav");
    audio->jump_land_sound = sound_load(sbox, audio, "res/sounds/jump_land.wav");
    audio->enter_water_sound = sound_load(sbox, audio, "res/sounds/enter_water.wav");
    audio->step_metal_sound = sound_load(sbox, audio, "res/sounds/step_metal.wav");
}

void a_free(sbox_t* sbox, audio_t* audio) {
    info(sbox, "shutting down audio...");

    int n = 0;
    sound_t* sound = audio->sounds;
    while (sound) {
        sound_t* next = sound->next;
        free(sound);
        sound = next;
        n++;
    }
    info(sbox, "released %d sounds", n);

    ALCdevice* device = alcGetContextsDevice(audio->context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(audio->context);
    alcCloseDevice(device);
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

void a_play(sbox_t* sbox, audio_t* audio, sound_t* sound, float pitch) {
    if (!sound) return;
    ALenum err;

    alSourcef(sound->source, AL_PITCH, pitch);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_PITCH: %d", err);

    alSourcef(sound->source, AL_GAIN, 1.0f);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_GAIN: %d", err);
    
    alSource3f(sound->source, AL_POSITION, 0, 0, 0);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_POSITION: %d", err);
    
    alSource3f(sound->source, AL_VELOCITY, 0, 0, 0);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_VELOCITY: %d", err);
    
    alSourcei(sound->source, AL_LOOPING, AL_FALSE);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_LOOPING: %d", err);

    alSourcePlay(sound->source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to play audio source: %d", err);
}

sound_t* sound_load(sbox_t* sbox, audio_t* audio, const char* path) {
    ALuint buffer;
    ALenum err;

    alGenBuffers(1, &buffer);
    if ((err = alGetError()) != AL_NO_ERROR) {
        error(sbox, "failed to generate audio buffer for %s: %d", path, err);
        return NULL;
    }

    SDL_AudioSpec spec;
    uint8_t* data = NULL;
    uint32_t len = 0;
    
    if (!SDL_LoadWAV(path, &spec, &data, &len)) {
        error(sbox, "failed to load %s: %s", path, SDL_GetError());
        return NULL;
    }

    len = len - len % 4;
    alBufferData(buffer, AL_FORMAT_STEREO16, data, len, spec.freq);
    if ((err = alGetError()) != AL_NO_ERROR) {
        error(sbox, "failed to set audio buffer data for %s: %d", path, err);
        return NULL;
    }

    SDL_FreeWAV(data);

    ALuint source;
    alGenSources(1, &source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to generate audio source for %s: %d", path, err);

    alSourcei(source, AL_BUFFER, buffer);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to set AL_BUFFER on source for %s: %d", path, err);

    sound_t* sound = malloc(sizeof(sound_t));
    sound->buffer = buffer;
    sound->source = source;
    sound->next = audio->sounds;
    audio->sounds = sound;
    return sound;
}

void sound_free(sbox_t* sbox, audio_t* audio, sound_t* sound) {
    if (!sound) return;

    ALenum err;
    alDeleteSources(1, &sound->source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to delete audio source: %d", err);
        
    alDeleteBuffers(1, &sound->buffer);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(sbox, "failed to delete audio buffer: %d", err);
}
