#include <SFML/Audio.hpp>
#include "Headers/SoundManage.h"
#include <iostream>

SoundManage::SoundManage(const std::string& filename): m_SoundBuffer(), m_Sound(m_SoundBuffer) {
	loadSound(filename);
}
void SoundManage::loadSound(const std::string& filename) {
	if (!m_SoundBuffer.loadFromFile(filename)) {
		std::cerr << "Failed to load " << filename << std::endl;
	}
	m_Sound.setBuffer(m_SoundBuffer);
}

void SoundManage::playSound() {
	m_Sound.play();
}