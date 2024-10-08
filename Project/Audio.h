#pragma once

#include <iostream>
#include <SFML/Audio.hpp>

class Audio
{
public:
    static sf::SoundBuffer* bgAudioBuff1;
    static sf::SoundBuffer* bgAudioBuff2;
    static sf::SoundBuffer* bgAudioBuff3;
    static sf::SoundBuffer* bgAudioBuff4;
    static sf::Sound* bgAudioSound;
    static sf::Sound* soundEffect;
    static bool IsBgAudioOn;
    static int currentAudioId;
    static bool BgAudioChangeStatus;
    static float bgAudioVolume;
    static void InitBgMusic();
    static void Cleanup();
    static void PlayBgMusic();
    static void PauseBgMusic();
    static void SetBgMusicVolume(float volume);
    static void CheckForAudioSwap(float distance, float yLvl);
    static void PlaySoundEffect(float distance, bool* endGame);
};