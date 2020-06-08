#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML\Graphics.hpp>
#include <array>
#include <bitset>
#include "chip8.h"

class display
{
public:
	display();
	void updateScreen(std::bitset<screenHeight * screenWidth>& screenBufferIn);
	void processEvents();
	bool isOpen() {return window.isOpen();}
	void dumpScreenBuffer() const;
private:
	const unsigned screenWidth {64};
	const unsigned screenHeight {32};
	const float pixelSize {8.0};
	sf::Color colorOn = sf::Color::White;
	sf::Color colorOff = sf::Color::Black;
	std::array<sf::RectangleShape, 32*64> pixels;
	std::bitset<32*64> screenBuffer;
	sf::RenderWindow window;
};

#endif