#include <iostream>	// For debug purposes.
#include <iomanip>
#include <fstream>
#include <bitset>
#include "chip8.h"
#include "dataTypes.h"
#include "opcodes.h"
#include "characterSet.h"

chip8::chip8()
{
	// Initialize memory.
	registers.fill(0);
	stack.fill(0);
	screenBuffer.reset();	// bitset.reset sets all bits to 0.
	memory.fill(0);
	loadCharacterSet();
}


void chip8::step()
{
	// Fetch next opcode and increment program counter.
	dbyte opcode = memory[programCounter] << 8 | memory[programCounter + 1];
	
	// Main interpreter.
	switch (opcode & 0xF000)
	{
		case opcodes::RET:
			programCounter = stack[--stackPointer];
			break;
		case opcodes::LDI:
			indexRegister = opcode & 0x0FFF;
			programCounter += 2;
			break;
		case opcodes::JP:
			programCounter = opcode & 0x0FFF;
			break;
		case opcodes::CALL:
			stack[stackPointer] = programCounter;
			++stackPointer;
			programCounter = opcode & 0x0FFF;
			break;
		case opcodes::LD:
			registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			programCounter += 2;
			break;
		case opcodes::DRW:
			// Display n-byte sprite *starting at memory location indexRegister* at (Vx, Vy), set VF = collision.
			// Save x coord.
			unsigned xCoord {registers[(opcode & 0x0F00) >> 8]};
			// Save y coord.
			unsigned yCoord {registers[(opcode & 0x00F0) >> 4]};
			// Save height of sprite.
			unsigned spriteHeight {static_cast<unsigned>(opcode & 0x000F)};
			
			// Set VF register to 0.
			registers[0xF] = 0;	// Switch to 1 if collision detected.
		
			for (unsigned rowIndex {}; rowIndex < spriteHeight; ++rowIndex)
			{
				std::bitset<8> spriteRow {memory[indexRegister + rowIndex]};
				for (unsigned bitIndex {}; bitIndex < 8; ++bitIndex)
				{
					// Get proper index for screen buffer.
					unsigned bufferIndex {static_cast<unsigned>(((yCoord + rowIndex) % screenHeight)*screenWidth + ((xCoord + bitIndex) % screenWidth))};
					if (spriteRow[7 - bitIndex] == true)
					{
						// If bit is already 1 then colission detected. Set VF to 1.
						if (screenBuffer[bufferIndex] == true) {registers[0xF] = 1;}
						// Xor correct bit in bufferIndex with sprite.
						screenBuffer[bufferIndex] = screenBuffer[bufferIndex] ^ spriteRow[7 - bitIndex];
					}
				}
			}
			drawFlag = true;
			programCounter += 2;
			break;
	}
	
}

void chip8::loadProgramFromMem(const std::vector<byte>& program)
{
	dbyte start {0x200};
	
	for (unsigned short index {}; index < program.size(); ++index)
	{
		memory[start + index] = program[index];
	}
}

void chip8::debug() const
{
	// Output registers.
	std::cout << " - Registers -\n";
	std::cout << "I: " << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << static_cast<short>(indexRegister) << std::endl; 
	std::cout << "Program Counter: " << std::setw(4) << std::hex << std::uppercase << static_cast<short>(programCounter) << std::endl; 
	for (unsigned int index {}; index < registers.size(); ++index)
	{
		std::cout << "V" << std::hex << std::uppercase << index << ":" << std::setw(2) << std::right << std::hex << static_cast<short>(registers[index]) << ' ';
		if (index % 2) { std::cout << std::endl; }
	}
	std::cout << std::endl;
}

void chip8::dumpMemory() const
{
	// Dump registers.
	debug();
	// Dump video buffer.
	dumpScreenBuffer();
	// Dump memory.
	std::cout << " - Memory Dump -\n";
	unsigned short cols {1};
	for (const auto& pMem : memory)
	{
		std::cout << std::setw(2) << std::hex << std::setfill('0') << static_cast<short>(pMem) << " ";
		if (!(cols++ % 16)) {std::cout << std::endl;}
	}
}


void chip8::dumpScreenBuffer() const
{
	// Dump contents of video buffer to standard out by row.
	// Print 1 for true. 0 for false.
	std::cout << " - Video Buffer - " << std::endl;
	for (size_t index {}; index < screenBuffer.size(); ++index)
	{
		std::cout << ( screenBuffer[index] ? "*" : "0");
		if (!((index + 1) % screenWidth)) {std::cout << std::endl;}
	}
	std::cout << std::endl;
}

void chip8::loadCharacterSet()
{
	// Write characters to location 0x000 to 0x1FF
	for (size_t index {}; index < characterSet::characters.size(); ++index)
	{
		memory[index] = characterSet::characters[index];
	}
}


std::bitset<screenHeight * screenWidth>& chip8::getScreenBuffer()
{
	drawFlag = false;
	return screenBuffer;
}
	
bool chip8::screenBufferHasChanged() const
{
	return drawFlag;
}