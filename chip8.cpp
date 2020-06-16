#include <iostream>	// For debug purposes.
#include <iomanip>
#include <fstream>
#include <bitset>
#include <exception>
#include <cstdlib>	// rand and srand.
#include <ctime>	// to seed rand.
#include "chip8.h"
#include "dataTypes.h"
#include "characterSet.h"

chip8::chip8()
{
	// Initialize memory.
	registers.fill(0);
	stack.fill(0);
	screenBuffer.reset();	// bitset.reset sets all bits to 0.
	memory.fill(0);
	loadCharacterSet();
	
	// Seed random number generator.
	srand(time(nullptr));
}


/// /brief Main program interpreter.
/// Interprets each two byte block of memory and performs required operation.
void chip8::step()
{
	/// Fetch next opcode and increment program counter.
	dbyte opcode = memory[programCounter] << 8 | memory[programCounter + 1];
	
	/// /brief Interprets opcodes.
	/// 16 different types of opcodes.  Each type can have more than one secondary functions.
	switch (opcode & 0xF000)
	{
		case 0x0000:
			switch (opcode & 0x00FF)
			{
				case 0x00E0:
					// / /brief 00E0 -  CLS
					// / Clears the screen.
					screenBuffer.reset();
					break;
				case 0x00EE:
					// / /brief 00EE - RET
					// / Return from a subroutine.  Set PC to address at the top of the stack and then
					// / subtracts 1 from the stack pointer.
					programCounter = stack[--stackPointer];
					break;
				default:
					break;
			}
			break;
		case 0x1000:
			/// /brief 1nnn - JP addr
			/// Jump to location nnn.
			/// Set program counter to nnn.
			programCounter = opcode & 0x0FFF;
			break;
		case 0x2000:
			/// /brief 2nnn - CALL addr
			/// The interpreter increments the stack pointer, then puts the current PC on the top 
			///	of the stack. The PC is then set to nnn.
			stack[stackPointer] = programCounter;
			++stackPointer;
			programCounter = opcode & 0x0FFF;
			break;
		case 0x3000:
			/// /brief 3xkk - SE Vx, byte
			/// Skip next instruction if Vx = kk.
			/// The interpreter compares register Vx to kk, and if they are equal, 
			/// increments the program counter by 2.
			if (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				programCounter += 4;
			else
				programCounter += 2;
			break;
		case 0x4000:
			/// /brief 4xkk - SNE Vx, byte
			/// Skip next instruction if Vx != kk.
			/// The interpreter compares register Vx to kk, and if they are not equal, 
			/// increments the program counter by 2.
			if (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				programCounter += 4;
			else
				programCounter += 2;
			break;
		case 0x5000:
			/// /brief 5xy0 - SE Vx, Vy
			/// Skip next instruction if Vx = Vy.
			/// The interpreter compares register Vx to register Vy, and if they are equal, 
			/// increments the program counter by 2.
			if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
				programCounter += 4;
			else
				programCounter += 2;
			break;
		case 0x6000:
			/// /brief 6xkk - LD Vx, byte
			/// Set Vx = kk.
			/// The interpreter puts the value kk into register Vx.
			registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			programCounter += 2;
			break;
		case 0x7000:
			/// /brief 7xkk - ADD Vx, byte
			/// Set Vx = Vx + kk.
			/// Adds the value kk to the value of register Vx, then stores the result in Vx.
			registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			programCounter += 2;
			break;
		case 0x8000:
			/// /brief This range of opcodes deal with bitwise operations on registers.
			switch (opcode & 0x000F)
			{
				case 0x0000:
					/// /brief 8xy0 - LD Vx, Vy
					/// Set Vx = Vy.
					/// Stores the value of register Vy in register Vx.
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
					break;
				case 0x0001:
					/// /brief 8xy1 - OR Vx, Vy
					/// Set Vx = Vx OR Vy.
					/// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
					registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
					break;
				case 0x0002:
					/// /brief 8xy2 - AND Vx, Vy
					/// Set Vx = Vx AND Vy.
					/// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
					registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
					break;
				case 0x0003:
					/// /brief 8xy3 - XOR Vx, Vy
					/// Set Vx = Vx XOR Vy.
					/// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
					registers[(opcode & 0x0F00) >> 8] ^= registers[(opcode & 0x00F0) >> 4];
					break;
				case 0x0004: {
					/// /brief 8xy4 - ADD Vx, Vy
					/// Set Vx = Vx + Vy, set VF = carry.
					/// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) 
					/// VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
					short registerX {(opcode & 0x0F00) >> 8};		// Registers.
					short registerY {(opcode & 0x00F0) >> 8};
					unsigned short valueX {registers[registerX]};	// Value of register x.
					unsigned short valueY {registers[registerY]};	// Value of register y.
					valueX += valueY;
					
					/// Check for overflow.
					if (valueX > 0xFF)
						registers[0xF] = 1;
					else
						registers[0xF] = 0;
					
					// Copy result back to register x.
					registers[registerX] = static_cast<byte>(valueX);
					} break;
				case 0x0005: {
					/// /brief 8xy5 - SUB Vx, Vy
					/// Set Vx = Vx - Vy, set VF = NOT borrow.
					/// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, 
					/// and the results stored in Vx.
					short registerX {(opcode & 0x0F00) >> 8};		// Registers.
					short registerY {(opcode & 0x00F0) >> 8};
					unsigned short valueX {registers[registerX]};	// Value of register x.
					unsigned short valueY {registers[registerY]};	// Value of register y.
					
					if (valueX > valueY)
						registers[0xF] = 1;
					else
						registers[0xF] = 0;
					valueX -= valueY;
					
					// Copy result back to register x.
					registers[registerX] = static_cast<byte>(valueX);
					} break;
				case 0x0006:
					/// /brief 8xy6 - SHR Vx {, Vy}
					/// Set Vx = Vx SHR 1.
					/// Shift right. If the least-significant bit of Vx is 1, then VF is set to 1, 
					/// otherwise 0. Then Vx is divided by 2.
					if (registers[(opcode & 0x0F00)] & 0x0001)
						registers[0xF] = 1;
					else
						registers[0xF] = 0;
					registers[(opcode & 0x0F00)] >> 1;	/// Shifting bits to right 1 is same as dividing by 1.
					break;
				case 0x0007: {
					/// /brief 8xy7 - SUBN Vx, Vy
					/// Set Vx = Vy - Vx, set VF = NOT borrow.
					/// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the 
					/// results stored in Vx.
					short registerX {(opcode & 0x0F00) >> 8};		// Registers.
					short registerY {(opcode & 0x00F0) >> 8};
					unsigned short valueX {registers[registerX]};	// Value of register x.
					unsigned short valueY {registers[registerY]};	// Value of register y.
					
					if (valueY > valueX)
						registers[0xF] = 1;
					else
						registers[0xF] = 0;
					valueX -= valueY;
					
					// Copy result back to register x.
					registers[registerX] = static_cast<byte>(valueX);
					} break;
				case 0x000E:
					/// /brief 8xyE - SHL Vx {, Vy}
					/// Set Vx = Vx SHL 1.
					/// Shift left.  If the most-significant bit of Vx is 1, then VF is set to 1, 
					/// otherwise to 0. Then Vx is multiplied by 2.
					if (registers[(opcode & 0x0F00)] & 0x8000)
						registers[0xF] = 1;
					else
						registers[0xF] = 0;
					registers[(opcode & 0x0F00)] << 1;	/// Shifting bits to left 1 is same as dividing by 1.
					break;
				default:
					break;
				}
			break;
		case 0x9000:
			/// /brief 9xy0 - SNE Vx, Vy
			/// Skip next instruction if Vx != Vy.
			/// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
			if (registers[opcode & 0x0F00] != registers[opcode & 0x00F0])
				indexRegister += 4;
			else
				indexRegister += 2;
			break;
		case 0xA000:			
			/// /brief Annn - LD I, addr
			/// Set I = nnn.
			/// The value of register I is set to nnn.
			indexRegister = opcode & 0x0FFF;
			programCounter += 2;
			break;
		case 0xB000:
			/// /brief Bnnn - JP V0, addr
			/// Jump to location nnn + V0.
			/// The program counter is set to nnn plus the value of V0.
			indexRegister = (opcode & 0x0FFF) + registers[0];
			break;
		case 0xC000: {
			/// /brief Cxkk - RND Vx, byte
			/// Set Vx = random byte AND kk.
			/// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. 
			/// The results are stored in Vx.
			byte randomNumber {rand() % 255};
			randomNumber &= (opcode & 0x00FF);
			registers[opcode & 0x0F00] = randomNumber;
			} break;
		case 0xD000: {
			/// /brief Dxyn - DRW Vx, Vy, nibble
			/// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			/// The interpreter reads n bytes from memory, starting at the address stored in I. These 
			/// bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed 
			/// onto the existing screen. If this causes any pixels to be erased, VF is set to 1, 
			/// otherwise it is set to 0. If the sprite is positioned so part of it is outside the 
			///coordinates of the display, it wraps around to the opposite side of the screen. 
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
			} break;
		case 0xE000:
			/* TODO */
			break;
		case 0xF000:
			/* TODO */
			break;
		default:
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