#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SDL2/SDL_mixer.h>

class AudioPlayer {
    public:
        AudioPlayer() = default;
        ~AudioPlayer();

        void init();
        void play_beep_sound();
    private:
        Mix_Chunk* m_beep_sound;
};

#endif
