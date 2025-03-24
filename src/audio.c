#include "audio.h"

void audio_init(Audio* audio) {
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, MIX_DEFAULT_CHANNELS, 2048) < 0) {
        printf("[ERROR] Unable to initialize SDL2_mixer !\n");
        exit(EXIT_FAILURE);
    }

    audio->beep_sound = Mix_LoadWAV("beep_sound.wav");

    if (audio->beep_sound == NULL) {
        printf("[ERROR] Unable to initialize the beep sound\n");
        exit(EXIT_FAILURE);
    }

    Mix_VolumeChunk(audio->beep_sound, MIX_MAX_VOLUME / 2);
}
void audio_play_beep_sound(Audio* audio) {
    int check = Mix_PlayChannel(-1, audio->beep_sound, 0);
    if (check < 0) {
        printf("[ERROR] Unable to play the beep sound !\n");
        exit(EXIT_FAILURE);
    }
}

void audio_deinit(Audio* audio) {
    if (audio->beep_sound)
        free(audio->beep_sound);
    Mix_CloseAudio();
}
