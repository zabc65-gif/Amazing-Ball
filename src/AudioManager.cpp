#include "AudioManager.hpp"
#include <iostream>

AudioManager::AudioManager() : initialized(false) {}

AudioManager::~AudioManager() {
    cleanup();
}

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::init() {
    if (initialized) {
        return true;
    }

    // Initialiser SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Erreur Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }

    // Allouer 16 canaux pour les effets sonores
    Mix_AllocateChannels(16);

    initialized = true;
    return true;
}

void AudioManager::cleanup() {
    if (!initialized) {
        return;
    }

    // Arrêter la musique
    stopMusic();

    // Libérer toutes les musiques
    for (auto& pair : musicTracks) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    musicTracks.clear();

    // Libérer tous les effets sonores
    for (auto& pair : soundEffects) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    soundEffects.clear();

    // Fermer SDL_mixer
    Mix_CloseAudio();
    initialized = false;
}

bool AudioManager::loadMusic(MusicTrack track, const std::string& filepath) {
    Mix_Music* music = Mix_LoadMUS(filepath.c_str());
    if (!music) {
        std::cerr << "Erreur Mix_LoadMUS (" << filepath << "): " << Mix_GetError() << std::endl;
        return false;
    }

    // Libérer l'ancienne musique si elle existe
    auto it = musicTracks.find(track);
    if (it != musicTracks.end() && it->second) {
        Mix_FreeMusic(it->second);
    }

    musicTracks[track] = music;
    return true;
}

void AudioManager::playMusic(MusicTrack track, int loops) {
    auto it = musicTracks.find(track);
    if (it == musicTracks.end() || !it->second) {
        std::cerr << "Musique non chargée" << std::endl;
        return;
    }

    if (Mix_PlayMusic(it->second, loops) == -1) {
        std::cerr << "Erreur Mix_PlayMusic: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

void AudioManager::pauseMusic() {
    Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
    Mix_ResumeMusic();
}

void AudioManager::setMusicVolume(int volume) {
    // Assurer que le volume est entre 0 et 128
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_VolumeMusic(volume);
}

bool AudioManager::loadSound(SoundEffect effect, const std::string& filepath) {
    Mix_Chunk* sound = Mix_LoadWAV(filepath.c_str());
    if (!sound) {
        std::cerr << "Erreur Mix_LoadWAV (" << filepath << "): " << Mix_GetError() << std::endl;
        return false;
    }

    // Libérer l'ancien son s'il existe
    auto it = soundEffects.find(effect);
    if (it != soundEffects.end() && it->second) {
        Mix_FreeChunk(it->second);
    }

    soundEffects[effect] = sound;
    return true;
}

void AudioManager::playSound(SoundEffect effect, int loops) {
    auto it = soundEffects.find(effect);
    if (it == soundEffects.end() || !it->second) {
        std::cerr << "Effet sonore non chargé" << std::endl;
        return;
    }

    // Jouer sur le premier canal disponible (-1)
    if (Mix_PlayChannel(-1, it->second, loops) == -1) {
        std::cerr << "Erreur Mix_PlayChannel: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::setSoundVolume(int volume) {
    // Assurer que le volume est entre 0 et 128
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    // Applique le volume à tous les canaux (-1)
    Mix_Volume(-1, volume);
}
