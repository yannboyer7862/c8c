#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>

typedef struct {
    Mix_Chunk* beep_sound;
} Audio;

void audio_init(Audio* audio);
void audio_play_beep_sound(Audio* audio);
void audio_deinit(Audio* audio);

#endif
