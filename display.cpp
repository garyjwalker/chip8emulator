#include <iostream>
#include "display.h"

display::display()
{
	// create window.
	window.create(sf::VideoMode(screenWidth * 8, screenHeight * 8), "Gary's Chip8 Emulator", sf::Style::Close);
	
	// Set properties of the "pixels."
	for (int index {}; index < pixels.size(); ++index)
	{
		pixels[index].setSize(sf::Vector2f(pixelSize,pixelSize));
		pixels[index].setFillColor(colorOff);
		pixels[index].setOutlineColor(sf::Color(0,100,0));
		pixels[index].setOutlineThickness(1);
	}
	
	// Set position for pixels.
	for (int index {},row {}; row < screenHeight; ++row)
	{
		for (int col {}; col < screenWidth; ++col)
		{
			pixels[index++].setPosition(sf::Vector2f(col * pixelSize,row * pixelSize));
		}
	}
}

void display::updateScreen(std::bitset<64 * 32>& screenBufferIn)
{
	// Copies screenbuffer to screen screenbuffer.
	for (size_t index {}; index < screenBuffer.size(); ++index)
	{
		screenBuffer[index] = screenBufferIn[index];
	}
}


void display::processEvents()
{
	// Handle events.
	sf::Event event;
	while (window.pollEvent(event)) {

		switch (event.type) 
		{
			case sf::Event::EventType::Closed:
				window.close();
				break;
		}
	}
	window.clear();
	// Update and draw the screen pixels.
	for (size_t index {}; index < screenBuffer.size(); ++index)
	{
		if (screenBuffer[index]) pixels[index].setFillColor(colorOn);
		else pixels[index].setFillColor(colorOff);
		window.draw(pixels[index]);
	}
	window.display();
}


void display::dumpScreenBuffer() const
{
	// Dump contents of video buffer to standard out by row.
	// Print * for true. 0 for false.
	std::cout << " - Video Buffer - " << std::endl;
	for (size_t index {}; index < screenBuffer.size(); ++index)
	{
		std::cout << ( screenBuffer[index] ? "*" : "0");
		if (!((index + 1) % screenWidth)) {std::cout << std::endl;}
	}
	std::cout << std::endl;
}