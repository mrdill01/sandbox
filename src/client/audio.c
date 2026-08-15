#include "audio.h"
#include "quark.h"
#include "player.h"
#include "render.h"

#include <SDL2/SDL_audio.h>

#ifndef QUARK_NO_AUDIO

#define REFERENCE_DISTANCE 3.0f
#define MAX_DISTANCE 20.0f

static void list_audio_devices(quark_t* quark, const ALCchar *devices) {
    const ALCchar* device = devices;
    const ALCchar* next = devices + 1;
    size_t len = 0;
    int num = 1;

    info(quark, "available audio devices: ");
    while (device && *device != '\0' && next && *next != '\0') {
        info(quark, "  %d. %s", num, device);
        len = strlen(device);
        device += len + 1;
        next += len + 2;
        num++;
    }
}

void a_init(quark_t* quark, audio_t* audio) {
    info(quark, "a_init()...");

    alGetError();
    
    ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        error(quark, "enumerating audio devices is not supported");

    list_audio_devices(quark, alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    ALCenum err;

    audio->device = alcOpenDevice((strcmp(a_device.string, "(null)") == 0) ? NULL : a_device.string);
    if (!audio->device || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(quark, "failed to open audio device: %d", err);
        return;
    }

    info(quark, "opened audio device...");

    audio->context = alcCreateContext(audio->device, NULL);
    if (!audio->context || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(quark, "failed to create OpenAL context: %d", err);
        return;
    }

    info(quark, "created audio context...");

    if (!alcMakeContextCurrent(audio->context) || ((err = alcGetError(NULL)) != ALC_NO_ERROR)) {
        error(quark, "failed to make OpenAL context current: %d", err);
        return;
    }

    info(quark, "made audio context current...");
    info(quark, "audio initialized!");

    audio->sounds = NULL;

    audio->sounds_playing = 0;

    audio->jump_sound = sound_load(quark, audio, "res/sounds/jump.wav");
    audio->jump_land_base_sound = sound_load(quark, audio, "res/sounds/jump_land_base.wav");

    for (int i = 0; i < NUM_PHYS_MAT; i++)
        audio->jump_land_sounds[i] = NULL;
    
    audio->jump_land_sounds[PHYS_MAT_METAL] =
        sound_load(quark, audio, "res/sounds/jump_land_metal.wav");
    audio->jump_land_sounds[PHYS_MAT_WOOD] =
        sound_load(quark, audio, "res/sounds/jump_land_wood.wav");
    audio->jump_land_sounds[PHYS_MAT_STONE] =
        sound_load(quark, audio, "res/sounds/jump_land_stone.wav");
    audio->jump_land_sounds[PHYS_MAT_GRASS] =
        sound_load(quark, audio, "res/sounds/jump_land_grass.wav");

    for (int i = 0; i < NUM_PHYS_MAT; i++)
        audio->step_sounds[i] = NULL;

    audio->step_sounds[PHYS_MAT_METAL] = sound_load(quark, audio, "res/sounds/step_metal.wav");
    audio->step_sounds[PHYS_MAT_WOOD] = sound_load(quark, audio, "res/sounds/step_wood.wav");
    audio->step_sounds[PHYS_MAT_STONE] = sound_load(quark, audio, "res/sounds/step_stone.wav");
    audio->step_sounds[PHYS_MAT_SAND] = sound_load(quark, audio, "res/sounds/step_sand.wav");
    audio->step_sounds[PHYS_MAT_GRASS] = sound_load(quark, audio, "res/sounds/step_grass.wav");
    audio->step_sounds[PHYS_MAT_WATER] = sound_load(quark, audio, "res/sounds/step_water.wav");

    audio->bullet_hit_sounds[PHYS_MAT_METAL] =
        sound_load(quark, audio, "res/sounds/bullet_hit_metal.wav");
    audio->bullet_hit_sounds[PHYS_MAT_WOOD] =
        sound_load(quark, audio, "res/sounds/bullet_hit_wood.wav");
    audio->bullet_hit_sounds[PHYS_MAT_STONE] =
        sound_load(quark, audio, "res/sounds/bullet_hit_stone.wav");
    audio->bullet_hit_sounds[PHYS_MAT_SAND] =
        sound_load(quark, audio, "res/sounds/bullet_hit_sand.wav");
    audio->bullet_hit_sounds[PHYS_MAT_GRASS] =
        sound_load(quark, audio, "res/sounds/bullet_hit_grass.wav");
    audio->bullet_hit_sounds[PHYS_MAT_WATER] =
        sound_load(quark, audio, "res/sounds/bullet_hit_stone.wav");
    audio->bullet_hit_sounds[PHYS_MAT_PLAYER] =
        sound_load(quark, audio, "res/sounds/bullet_hit_player.wav");

    for (int i = 1; i <= NUM_SPEECH_SOUNDS; i++) {
        char path[64];
        sprintf(path, "res/sounds/speech/%d.wav", i);
        audio->speech_sounds[i] = sound_load(quark, audio, path);
    }

    audio->enter_water_sound = sound_load(quark, audio, "res/sounds/enter_water.wav");
    audio->exit_water_sound = sound_load(quark, audio, "res/sounds/exit_water.wav");
    audio->explosion_sound = sound_load(quark, audio, "res/sounds/weapons/explosion.wav");
    audio->gun_click_sound = sound_load(quark, audio, "res/sounds/weapons/gun_click.wav");
    audio->pickup_coin_sound = sound_load(quark, audio, "res/sounds/pickup_coin.wav");
    audio->hurt_sound = sound_load(quark, audio, "res/sounds/hurt.wav");
    audio->fall_damage_sound = sound_load(quark, audio, "res/sounds/fall_damage.wav");
    audio->inventory_open_sound = sound_load(quark, audio, "res/sounds/inventory_open.wav");
    audio->inventory_close_sound = sound_load(quark, audio, "res/sounds/inventory_close.wav");

    audio->hotbar_select_sounds[0] = sound_load(quark, audio, "res/sounds/hotbar_select.wav");
    audio->hotbar_select_sounds[1] = sound_load(quark, audio, "res/sounds/hotbar_select2.wav");
    audio->hotbar_select_sounds[2] = sound_load(quark, audio, "res/sounds/hotbar_select3.wav");
    audio->hotbar_select_sounds[3] = sound_load(quark, audio, "res/sounds/hotbar_select4.wav");

    audio->button_hover_sound = sound_load(quark, audio, "res/sounds/button_hover.wav");
    audio->button_press_sound = sound_load(quark, audio, "res/sounds/button_press.wav");
}

void a_free(quark_t* quark, audio_t* audio) {
    info(quark, "a_free()...");

    int n = 0;
    sound_t* sound = audio->sounds;
    while (sound) {
        sound_t* next = sound->next;
        free(sound);
        sound = next;
        n++;
    }
    info(quark, "released %d sounds", n);

    ALCdevice* device = alcGetContextsDevice(audio->context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(audio->context);
    alcCloseDevice(device);
    info(quark, "audio shut down!");
}

void a_tick(quark_t* quark, audio_t* audio, player_t* player, camera_t* camera) {
    ALenum err;

    alListenerf(AL_GAIN, a_volume.value);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_GAIN: %d", err);

    if (camera) {
        alListener3f(AL_POSITION, camera->position[0], camera->position[1], camera->position[2]);
        if ((err = alGetError()) != AL_NO_ERROR)
            error(quark, "failed to set AL_POSITION: %d", err);
    
        ALfloat orientation[] = {
            camera->forward[0], camera->forward[1], camera->forward[2],
            camera->up[0], camera->up[1], camera->up[2],
        };
        alListenerfv(AL_ORIENTATION, orientation);
        if ((err = alGetError()) != AL_NO_ERROR)
            error(quark, "failed to set AL_ORIENTATION: %d", err);
    }

    if (player) {
        alListener3f(AL_VELOCITY, player->velocity[0], player->velocity[1], player->velocity[2]);
        if ((err = alGetError()) != AL_NO_ERROR)
            error(quark, "failed to set AL_VELOCITY: %d", err);
    }

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_LINEAR_DISTANCE_CLAMPED: %d", err);

    audio->sounds_playing = 0;
    sound_t* sound = audio->sounds;
    while (sound) {
        if (sound_is_playing(quark, sound))
            audio->sounds_playing++;
        sound = sound->next;
    }
}

void a_play(quark_t* quark, audio_t* audio, sound_t* sound, vec3 position, float pitch) {
    if (!sound) return;
    ALenum err;

    alSourcef(sound->source, AL_GAIN, 1.0f);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_GAIN: %d", err);

    alSourcef(sound->source, AL_PITCH, pitch * sv_timescale.value);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_PITCH: %d", err);
    
    alSource3f(sound->source, AL_POSITION, position[0], position[1], position[2]);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_POSITION: %d", err);
    
    alSource3f(sound->source, AL_VELOCITY, 0, 0, 0);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_VELOCITY: %d", err);
    
    alSourcei(sound->source, AL_LOOPING, AL_FALSE);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_LOOPING: %d", err);

    alSourcef(sound->source, AL_ROLLOFF_FACTOR, 1.0f);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_ROLLOFF_FACTOR: %d", err);
    
    alSourcef(sound->source, AL_REFERENCE_DISTANCE, REFERENCE_DISTANCE);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_REFERENCE_DISTANCE: %d", err);
    
    alSourcef(sound->source, AL_MAX_DISTANCE, MAX_DISTANCE);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_MAX_DISTANCE: %d", err);

    alSourcePlay(sound->source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to play audio source: %d", err);
}

int a_get_max_source_count(quark_t* quark, audio_t* audio) {
    int num = 0;
    ALenum err;

    alcGetIntegerv(audio->device, ALC_MONO_SOURCES, 1, &num);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to get ALC_MONO_SOURCES: %d", err);
    return num;
}

sound_t* sound_load(quark_t* quark, audio_t* audio, const char* path) {
    info(quark, "loading %s", path);

    ALuint buffer;
    ALenum err;

    alGenBuffers(1, &buffer);
    if ((err = alGetError()) != AL_NO_ERROR) {
        error(quark, "failed to generate audio buffer for %s: %d", path, err);
        return NULL;
    }

    SDL_AudioSpec spec;
    uint8_t* data = NULL;
    uint32_t len = 0;
    
    if (!SDL_LoadWAV(path, &spec, &data, &len)) {
        error(quark, "failed to load %s: %s", path, SDL_GetError());
        return NULL;
    }

    SDL_AudioCVT cvt;
    int status = SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
        spec.format, 1, spec.freq);

    if (status < 0) {
        error(quark, "failed to build audio converter: %s", SDL_GetError());
        return NULL;
    }

    cvt.len = len;
    cvt.buf = malloc(cvt.len * cvt.len_mult);
    if (cvt.buf == NULL) {
        error(quark, "failed to malloc buffer for converted audio");
        SDL_FreeWAV(data);
        return NULL;
    }

    memcpy(cvt.buf, data, cvt.len * cvt.len_mult);

    if (SDL_ConvertAudio(&cvt) < 0) {
        error(quark, "failed to convert audio: %s", SDL_GetError());
        free(cvt.buf);
        SDL_FreeWAV(data);
        return NULL;
    }

    alBufferData(buffer, AL_FORMAT_MONO16, cvt.buf, cvt.len * cvt.len_ratio, spec.freq);
    if ((err = alGetError()) != AL_NO_ERROR) {
        error(quark, "failed to set audio buffer data for %s: %d", path, err);
        return NULL;
    }

    SDL_FreeWAV(data);

    ALuint source;
    alGenSources(1, &source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to generate audio source for %s: %d", path, err);

    alSourcei(source, AL_BUFFER, buffer);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to set AL_BUFFER on source for %s: %d", path, err);

    sound_t* sound = malloc(sizeof(sound_t));
    sound->buffer = buffer;
    sound->source = source;
    sound->duration = ((double)(cvt.len * cvt.len_mult) / spec.channels) / spec.freq;
    sound->next = audio->sounds;
    audio->sounds = sound;
    return sound;
}

void sound_free(quark_t* quark, audio_t* audio, sound_t* sound) {
    if (!sound) return;

    ALenum err;
    alDeleteSources(1, &sound->source);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to delete audio source: %d", err);
        
    alDeleteBuffers(1, &sound->buffer);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to delete audio buffer: %d", err);
}

bool sound_is_playing(quark_t* quark, sound_t* sound) {
    ALint state;
    ALenum err;
    alGetSourcei(sound->source, AL_SOURCE_STATE, &state);
    if ((err = alGetError()) != AL_NO_ERROR)
        error(quark, "failed to get AL_SOURCE_STATE: %d", err);
    return state == AL_PLAYING;
}

#else

void a_init(quark_t* quark, audio_t* audio) {}
void a_free(quark_t* quark, audio_t* audio) {}
void a_tick(quark_t* quark, audio_t* audio, player_t* player, camera_t* camera) {}
void a_play(quark_t* quark, audio_t* audio, sound_t* sound, vec3 position, float pitch) {}
int a_get_max_source_count(quark_t* quark, audio_t* audio);

sound_t* sound_load(quark_t* quark, audio_t* audio, const char* path) {
    return NULL;
}

void sound_free(quark_t* quark, audio_t* audio, sound_t* sound) {}

bool sound_is_playing(quark_t* quark, sound_t* sound) {
    return false;
}

#endif
