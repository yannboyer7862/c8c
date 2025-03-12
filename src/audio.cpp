#include "audio.hpp"
#include <iostream>
#include <stdexcept>

AudioPlayer::~AudioPlayer() {
    if (m_beep_sound)
        free(m_beep_sound);
    Mix_CloseAudio();
}

void AudioPlayer::init() {
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, MIX_DEFAULT_CHANNELS, 2048) < 0)
        throw std::runtime_error("[ERROR] Unable to initialize SDL2_mixer !");

    m_beep_sound = Mix_LoadWAV("beep_sound.wav");

    if (m_beep_sound == nullptr)
        throw std::runtime_error("[ERROR] Unable to load the beep sound !");

    Mix_VolumeChunk(m_beep_sound, MIX_MAX_VOLUME / 2);
}

void AudioPlayer::play_beep_sound() {
    int check = Mix_PlayChannel(-1, m_beep_sound, 0);
    if (check < 0) {
        std::cerr << "[ERROR] Unable to play beep sound !\n";
        std::exit(1);
    }
}
