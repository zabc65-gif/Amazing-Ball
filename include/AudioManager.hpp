#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

enum class MusicTrack {
    MENU,
    GAMEPLAY,
    GAME_OVER
};

enum class SoundEffect {
    HOLE_FALL,
    LEVEL_COMPLETE,
    MENU_SELECT,
    JUMP,
    LAND,
    ATTACK
};

class AudioManager {
public:
    static AudioManager& getInstance();

    bool init();
    void cleanup();

    // Gestion de la musique
    bool loadMusic(MusicTrack track, const std::string& filepath);
    void playMusic(MusicTrack track, int loops = -1); // -1 = boucle infinie
    void stopMusic();
    void fadeOutMusic(int fadeTimeMs = 1000); // Fondu sortant (défaut 1 seconde)
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128

    // Gestion des effets sonores
    bool loadSound(SoundEffect effect, const std::string& filepath);
    void playSound(SoundEffect effect, int loops = 0); // 0 = une seule fois
    void setSoundVolume(int volume); // 0-128

private:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::map<MusicTrack, Mix_Music*> musicTracks;
    std::map<SoundEffect, Mix_Chunk*> soundEffects;

    bool initialized;
};

#endif
