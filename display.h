#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML\Graphics.hpp>
#include <array>
#include <bitset>
#include "chip8.h"
#include "screenProperties.h"

class display
{
public:
	display();
	void updateScreen(std::bitset<screenHeight * screenWidth>& screenBufferIn);
	void processEvents();
	bool isOpen() {return window.isOpen();}
	void dumpScreenBuffer() const;
private:
	const float pixelSize {8.0};
	const sf::Color colorOn {sf::Color::White};
	const sf::Color colorOff {sf::Color::Black};
	std::array<sf::RectangleShape, screenHeight * screenWidth> pixels;
	std::bitset<screenHeight * screenWidth> screenBuffer;
	sf::RenderWindow window;
};

#endif