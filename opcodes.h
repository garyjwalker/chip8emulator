#ifndef OPCODES_H
#define OPCODES_H

#include "dataTypes.h"

namespace opcodes
{
	const dbyte RET		= 0x00EE;	// RET
	const dbyte LDI		= 0xA000;	// LD I, addr	- Move addr into memory index I.
	const dbyte JP 		= 0x1000;	// JP addr		- Jump to addr.
	const dbyte CALL	= 0x2000;	// CALL addr	- Call subroutine at addr.
	const dbyte LD		= 0x6000;	// LD Vxkk		- Load byte kk into register Vx.
	const dbyte DRW		= 0xD000;	// DRW Vx Vy, nibble -  Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
}

#endif