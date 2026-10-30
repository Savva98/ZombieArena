#include <SFML/Graphics.hpp>
#include "Headers/Text.h"
using namespace sf;

TextWrapper::TextWrapper(Font &font, unsigned int characterSize, Color color, const std::string& text, Vector2f position): m_Font(font), m_Text(m_Font) {
	m_Text.setCharacterSize(characterSize);
	m_Text.setFillColor(color);
	m_Text.setString(text);
	m_Text.setPosition(position);
}

void TextWrapper::setText(const std::string& text) {
	m_Text.setString(text);
}

Text TextWrapper::getText() const {
	return m_Text;
}