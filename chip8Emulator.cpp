#include <iostream>
#include <vector>
#include "chip8.h"
#include "display.h"
#include "dataTypes.h"

int main (int argc, char *argv[])
{
	// Quick program.
	std::vector<byte> program;
	// Print 0 at 0,0.
	program.push_back(0xA0);	// LD I, 0x006 
	program.push_back(0x00);
	program.push_back(0x60);	// LD V0 0x02
	program.push_back(0x00);
	program.push_back(0x61);	// LD v1 0x02
	program.push_back(0x00);
	program.push_back(0xD0);	// DRW V0 V1 0x5
	program.push_back(0x15);
	// Print B at 3,5
	program.push_back(0xA0);	// LD I, 0x006 
	program.push_back(0x32);
	program.push_back(0x60);	// LD V0 0x03
	program.push_back(0x03);
	program.push_back(0x61);	// LD v1 0x04
	program.push_back(0x04);
	program.push_back(0xD0);	// DRW V0 V1 0x5
	program.push_back(0x15);
	// Print B overlapping edge.
	program.push_back(0x60);	// LD V0 0x3E
	program.push_back(0x3E);
	program.push_back(0x61);	// LD v1 0x1E
	program.push_back(0x1E);
	program.push_back(0xD0);	// DRW V0 V1 0x5
	program.push_back(0x15);
	
	
	chip8 processor;
	display screen;
	
	processor.loadProgramFromMem(program);
	
	size_t i {};
	while (screen.isOpen())
	{
		if (i < program.size())
		{
			processor.step();
			++i;
		}
		if (processor.screenBufferHasChanged())
		{
			screen.updateScreen(processor.getScreenBuffer());
		}
		screen.processEvents();
	}
	
	return 0;
}