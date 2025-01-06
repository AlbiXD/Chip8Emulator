#include "Chip8.cpp"
#include <iostream>
#include <fstream>
#include <iomanip> // For std::hex and std::setfill
#include <SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 10
void renderScreen(SDL_Renderer *renderer, const uint8_t *gfx)
{

	SDL_SetRenderDrawColor(renderer, 67, 28, 83, 2555); // SCREEN COLOR
	SDL_RenderClear(renderer);							// CLEAR THE SCREEN

	// SET THE DRAW COLOR
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			int index = y * 64 + x;

			if (gfx[index] == 1)
			{
				SDL_Rect pixel = {x * SCALE, y * SCALE, SCALE, SCALE};
				SDL_RenderFillRect(renderer, &pixel);
			}
		}
	}
	SDL_RenderPresent(renderer); // PRESENT THE FRAME
}

int main(int argc, char *argv[])
{
	Chip8 chip;

	const char *fileName = "pong.ch8";

	chip.loadROM(fileName);

	// LETS GET A SCREEN GOING ON
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "FAILED TO INITIALIZE SDL: " << SDL_GetError() << std::endl;
		return -1; // INDICATES THAT THE FIRST VALUE IS LESS THAN THE SECOND ONE
	}

	SDL_Window *window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN); // Initalizes the window
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window)
	{ // Error creating window
		SDL_Quit();
		return -1;
	}

	// PROGRAM RUNS UNTIL USER TERMINATES
	bool running = true;
	bool paused = false;

	uint32_t last_tick = SDL_GetTicks();
	while (running)
	{

		if (!paused)
		{
			chip.emulate_Cycle();

			if (chip.PC > 4096)
			{
				std::cerr << "PC OUT OF BOUND PAUSING CYCLE." << std::endl;
				paused = true;
			}
		}
		if (chip.draw_flag)
		{
			renderScreen(renderer, chip.gfx);
			chip.draw_flag = false;
		}

		if (chip.clearScreen_flag)
		{
			SDL_RenderClear(renderer);
			chip.clearScreen_flag = false;
		}

		/*
			CHIP-8 KEYBOARD

			1 | 2 | 3 | C
			-------------
			4 | 5 | 6 | D
			-------------
			7 | 8 | 9 | E
			-------------
			A | 0 | B | F






		*/
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{ // Event Handling Loop

			switch (event.type)
			{
			case (SDL_QUIT):
			{
				running = false;
				break;
			}
			case (SDL_KEYDOWN):
			{
				switch (event.key.keysym.sym)
				{
				case (SDLK_0):
				{
					chip.keys[0] = 1;
					std::cout << "======================= 0 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_1):
				{
					std::cout << "======================= 1 KEY PRESSED =======================" << std::endl;
					chip.keys[1] = 1;
					break;
				}
				case (SDLK_2):
				{
					std::cout << "======================= 2 KEY UNPRESSED =======================" << std::endl;
					chip.keys[2] = 1;
					break;
				}
				case (SDLK_3):
				{
					std::cout << "======================= 3 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_4):
				{
					std::cout << "======================= 4 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_5):
				{
					std::cout << "======================= 5 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_6):
				{
					std::cout << "======================= 6 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_7):
				{
					std::cout << "======================= 7 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_8):
				{
					std::cout << "======================= 8 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_9):
				{
					std::cout << "======================= 9 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				}
				break;
			}
			case (SDL_KEYUP):
			{
				switch (event.key.keysym.sym)
				{
				case (SDLK_0):
				{
					chip.keys[0] = 0;
					std::cout << "======================= 0 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_1):
				{
					std::cout << "======================= 1 KEY PRESSED =======================" << std::endl;
					chip.keys[1] = 0;
					break;
				}
				case (SDLK_2):
				{
					chip.keys[2] = 0;
					std::cout << "======================= 2 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_3):
				{
					std::cout << "======================= 3 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_4):
				{
					std::cout << "======================= 4 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_5):
				{
					std::cout << "======================= 5 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_6):
				{
					std::cout << "======================= 6 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_7):
				{
					std::cout << "======================= 7 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_8):
				{
					std::cout << "======================= 8 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				case (SDLK_9):
				{
					std::cout << "======================= 9 KEY UNPRESSED =======================" << std::endl;
					break;
				}
				}
				break;
			}
			}
		}
		SDL_Delay(3);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}