#ifndef TEXT_HPP
#define TEXT_HPP

#include <SFML/Graphics.hpp>

class Text {
	static sf::Font font;
	static sf::Text text;
	public:
	static void init() {
		font.loadFromFile("font.ttf");
		text.setFont(font);
	}
	static void drawText(sf::RenderWindow& w, std::string msg, sf::Vector2f position,
			int size = 16, bool center = true) {
		text.setCharacterSize(size);
		text.setString(msg);
		text.setFillColor(sf::Color::White);
		auto bounds = text.getLocalBounds();
		if(center)
			text.setPosition(position - sf::Vector2f(bounds.width / 2., bounds.height / 2));
		else
			text.setPosition(position);
		w.draw(text);
	}
};

#endif
