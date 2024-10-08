#include "Audio.h"

using std::cout;

// Audio
sf::SoundBuffer* Audio::bgAudioBuff1;
sf::SoundBuffer* Audio::bgAudioBuff2;
sf::SoundBuffer* Audio::bgAudioBuff3;
sf::SoundBuffer* Audio::bgAudioBuff4;
sf::Sound* Audio::bgAudioSound;
sf::Sound* Audio::soundEffect;
float Audio::bgAudioVolume = 10.0f;
int Audio::currentAudioId = 1;
bool Audio::IsBgAudioOn = true;
bool Audio::BgAudioChangeStatus = false;

void Audio::InitBgMusic()
{
	bgAudioBuff1 = new sf::SoundBuffer();
	bgAudioBuff2 = new sf::SoundBuffer();
	bgAudioBuff3 = new sf::SoundBuffer();
	bgAudioBuff4 = new sf::SoundBuffer();
	if (!bgAudioBuff1->loadFromFile("src/audio/forest_sound.wav"))
		cout << "\nAudio not loaded!";
	if (!bgAudioBuff2->loadFromFile("src/audio/droplets-in-a-cave-6785.wav"))
		cout << "\nAudio not loaded!";
	if (!bgAudioBuff3->loadFromFile("src/audio/nightmare_approaching_sound-26223.wav"))
		cout << "\nAudio not loaded!";
	if (!bgAudioBuff4->loadFromFile("src/audio/ghost-sigh3-99231_1.wav"))
		cout << "\nAudio not loaded!";
	bgAudioSound = new sf::Sound();
	soundEffect = new sf::Sound();
	bgAudioSound->setBuffer(*bgAudioBuff1);
	bgAudioSound->setVolume(8.f);
	bgAudioSound->setLoop(true);
	Audio::PlayBgMusic();
}

void Audio::Cleanup()
{
	bgAudioSound->stop();
	delete bgAudioSound;
	delete soundEffect;
	delete bgAudioBuff1;
	delete bgAudioBuff2;
	delete bgAudioBuff3;
	delete bgAudioBuff4;
}

void Audio::PlayBgMusic()
{
	bgAudioSound->play();
	BgAudioChangeStatus = false;
}

void Audio::PauseBgMusic()
{
	bgAudioSound->pause();
	BgAudioChangeStatus = false;
}

void Audio::SetBgMusicVolume(float volume)
{
	if (volume > 100 || volume < 0)
		return;
	bgAudioSound->setVolume(volume);
}

void Audio::CheckForAudioSwap(float distance, float yLvl)
{
	BgAudioChangeStatus = false;
	int trackToChange = 0;

	///
	if (distance < 60.f)
	{
		trackToChange = 1;
		BgAudioChangeStatus = true;
	}
	if (distance > 60.f)
	{
		trackToChange = 2;
		BgAudioChangeStatus = true;
	}
	if (yLvl < -140.f)
	{
		trackToChange = 3;
		BgAudioChangeStatus = true;
	}

	///

	if (BgAudioChangeStatus && trackToChange!=currentAudioId)
	{
		bgAudioSound->stop();
		switch (trackToChange)
		{
		case 1:
			bgAudioSound->setBuffer(*bgAudioBuff1);
			currentAudioId = 1;
			break;
		case 2:
			bgAudioSound->setBuffer(*bgAudioBuff2);
			currentAudioId = 2;
			break;
		case 3:
			bgAudioSound->setBuffer(*bgAudioBuff3);
			currentAudioId = 3;
			break;
		default: 
			break;
		}

		if(currentAudioId == 3)
			bgAudioSound->setVolume(Audio::bgAudioVolume*0.3f);
		else 
			bgAudioSound->setVolume(Audio::bgAudioVolume);

		bgAudioSound->setLoop(true);
		Audio::PlayBgMusic();
	}
}

void Audio::PlaySoundEffect(float distance, bool *endGame)
{
	static bool playEffect = false;
	if ((distance <= 50.0f)&&(playEffect == false))
	{
		soundEffect->setBuffer(*bgAudioBuff4);
		soundEffect->setVolume(Audio::bgAudioVolume*0.5f);
		soundEffect->play();
		playEffect = true;
		*endGame = true;
	}
}