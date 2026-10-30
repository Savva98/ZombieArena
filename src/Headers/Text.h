#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class TextWrapper {
private:
	Font m_Font;
	Text m_Text;
public:

	TextWrapper(Font &font, unsigned int characterSize, Color color, const std::string& text, Vector2f position);

	void setText(const std::string& text);
	Text getText() const;
};