#pragma once
#include <SFML/Audio.hpp>


class SoundManage {
private:
	sf::SoundBuffer m_SoundBuffer;
	sf::Sound m_Sound;
public:
	SoundManage(const std::string& filename);
	void loadSound(const std::string& filename);
	void playSound();
};