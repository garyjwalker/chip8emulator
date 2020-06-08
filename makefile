CC=g++
SFML_L=-LC:\SFML\lib
SFML_I=-IC:\SFML\include
SFML_FLAGS=-lsfml-graphics -lsfml-window -lsfml-system

all: chip8Emu.exe

chip8Emu.exe: chip8Emu.o chip8.o display.o
	$(CC) $(SFML_L) -o $@ $^ $(SFML_FLAGS)
	
chip8Emu.o: chip8Emulator.cpp
	$(CC) $(SFML_I) -c -o $@ $^

chip8.o: chip8.cpp
	$(CC) -c -o $@ $^

display.o: display.cpp
	$(CC) $(SFML_I) -c -o $@ $^ 
	
clean:
	del display.o chip8.o chip8Emu.o chip8Emu.exe