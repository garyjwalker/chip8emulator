// Emulator for chip8 emulator.

// - Memory -
// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
// 0x200-0xFFF - Program ROM and work RAM

#ifndef CHIP8_H
#define CHIP8_H

#include <vector>
#include <array>
#include <bitset>
#include "dataTypes.h"
#include "screenProperties.h"


const short sizeOfStack = 16;
const short sizeOfMemory = 4096;

const short numOfRegisters = 16;

class chip8
{
public:
	chip8();
	~chip8() = default;
	void step();
	void loadProgramFromMem(const std::vector<byte>& program);
	void debug() const;
	void dumpMemory() const;
	void dumpScreenBuffer() const;
	std::bitset<screenHeight * screenWidth>& getScreenBuffer();
	bool screenBufferHasChanged() const;
private:
	void loadCharacterSet();
	std::array<byte,  numOfRegisters> registers;
	std::array<dbyte, sizeOfStack>    stack;
	std::array<byte,  sizeOfMemory>   memory;
	std::bitset<screenHeight * screenWidth> screenBuffer;
	bool drawFlag {false};			// True if something needs to be drawn to screen.
	dbyte stackPointer {};			// Points ot top of stack.
	dbyte indexRegister {};			// Points to location in memory.
	dbyte programCounter {0x200};	// Points to next line to run.
};

#endif