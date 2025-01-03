#include "Chip8.cpp"
#include <iostream>
#include <fstream>
#include <iomanip> // For std::hex and std::setfill
#include <SDL.h>


#define WIDTH 64
#define HEIGHT 32
#define SCALE 10

int main(int argc, char* argv[])
{
	Chip8 chip;

	const char* fileName = "simpler.c8";

	chip.loadROM(fileName);

	//LETS GET A SCREEN GOING ON
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "FAILED TO INITIALIZE SDL: " << SDL_GetError() << std::endl;
		return -1; //INDICATES THAT THE FIRST VALUE IS LESS THAN THE SECOND ONE
	}

	SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_SHOWN); //Initalizes the window

	if (!window) { //Error creating window
		SDL_Quit();
		return -1;
	}

	//PROGRAM RUNS UNTIL USER TERMINATES
	bool running = true;
	bool paused = false;

	while (running) {

		if (!paused) {
			chip.emulate_Cycle();

			if (chip.PC > 4096) {
				std::cerr << "PC OUT OF BOUND PAUSING CYCLE." << std::endl;
				paused = true;
			}
		}


		SDL_Event event;
		while (SDL_PollEvent(&event)) { //Event Handling Loop
			if (event.type == SDL_QUIT)
				running = false;
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();



	return 0;
}
