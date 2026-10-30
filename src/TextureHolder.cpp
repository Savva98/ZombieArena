#include "Headers/TextureHolder.h"
#include <assert.h>

TextureHolder* TextureHolder::m_s_Instance = nullptr;
TextureHolder::TextureHolder() {
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(const std::string& filename) {
	auto& instance = m_s_Instance->m_Textures;
	auto pairFound = instance.find(filename);
	if (pairFound != instance.end()) {
		return pairFound->second;
	}
	else {
		auto& texture = instance[filename];
		if (!texture.loadFromFile(filename)) {
			throw std::runtime_error("TextureHolder::GetTexture - Failed to load " + filename);
		}
		return texture;
	}
}