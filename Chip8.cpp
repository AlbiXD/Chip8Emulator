#include <cstdint>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <iomanip> // For std::hex and std::setfill
#include <iostream>

struct Chip8
{
	uint8_t mem[4096]; // Memory is 4096 bytes, from location 0x000 to 0xFFF
	// The first 512 bytes, from 0x000 to 0x1FF are reserved;

	uint8_t V[16];              // 16 Registers with 8-bit (0-F in hexadecimal)
	uint16_t I;                 // Registor to store memory addresses
	uint16_t PC;                // Stores currently executing address
	uint8_t SP;                 // Stack pointer (Points at the top of the stack)
	uint16_t stack[16];         // Stack is an array of 16 16-bit values
	uint8_t gfx[64 * 32];   // Created a buffer for the graphics 64 by 
	bool draw_flag = false;
	bool clearScreen_flag = false;
	uint8_t keys[16];
	unsigned char x;
	Chip8() : I(0), PC(0x200), SP(0)
	{
		std::fill(std::begin(mem), std::end(mem), 0);
		std::fill(std::begin(V), std::end(V), 0);
		std::fill(std::begin(gfx), std::end(gfx), 0);
		std::fill(std::begin(stack), std::end(stack), 0);
		std::fill(std::begin(keys), std::end(keys), 0);
	}

	/*
		TODO: Load instructions into memory
	*/
	bool loadROM(const char* file_path)
	{
		std::ifstream file(file_path, std::ios::binary);

		if (!file)
		{ // Check if file exists
			std::cout << "Incorrect File Name" << std::endl;
			return false;
		}

		file.seekg(0, std::ios::end);           // Moves pointer to the end of the file
		std::streampos fileSize = file.tellg(); // Gets the byte position at the end
		file.seekg(0, std::ios::beg);           // Moves the pointer back to the beginning of the file in order to transfer bytes into memory

		if (fileSize > (4096 - 512)) // Checks if file is too large
		{
			std::cerr << "FILE SIZE TOO LARGE" << std::endl;
			return false;
		}

		if (!file.read(reinterpret_cast<char*>(mem + 512), fileSize))
		{ // Casts mem (int8_t*) to a char* and stores its values
			std::cerr << "Failed to read ROM into memory." << std::endl;
			return false;
		}

		file.close();
		return true;
	}

	void emulate_Cycle()
	{
		// FETCH
		uint8_t byte1, byte2;
		uint16_t opcode;
		byte1 = mem[PC];
		byte2 = mem[PC + 1];

		opcode = (byte1 << 8) | byte2; //COMBINE OPCODE IN MEMORY

		PC += 2; //INCREMENT PC BY 2

		// DECODE AND EXECUTE
		uint8_t instruction = opcode >> 12; // FIRST NIBBLE

		switch (instruction)
		{
		case(0x1): { //1nnn - JUMP INSTRUCTION
			uint16_t address = opcode & 0x0FFF;
			PC = address;
			std::cout << "JUMP INSTRUCTION TO ADDRESS " << PC << std::endl;
			break;
		}
		case(0x2): { //2nnn - CALLING SUBROUTINE AT ADDRESS nnn
			uint16_t address = opcode & 0x0FFF;
			std::cout << "CALLING SUBROUTINE TO " << address << std::endl;
			this->push(PC);
			PC = address;
			break;
		}
		case(0x3): { //3xkk SKIP INSTRUCTION IF Vx == kkk
			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t byte = opcode & 0xFF;      // EXTRACT BYTE VALUE

			if (V[vx] == byte) {
				PC += 4;
				break;
			}

			PC += 2;
			break;

		}
		case(0x4): { //4xkk SKIP INSTRUCTION IF Vx != kkk
			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t byte = opcode & 0xFF;      // EXTRACT BYTE VALUE

			if (V[vx] != byte) {
				PC += 4;
				break;
			}

			PC += 2;
			break;

		}
		case(0x5): ////5xy0 SKIP INSTRUCTION IF Vx == Vy
		{

			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t vy = (opcode >> 4) & 0x0F; // EXTRACT REGISTER NUMBER
			if (!((opcode & 0x000F) == 0)) {
				std::cout << "INVALID OPCODE FORMAT" << std::endl;
				return;
			}
			if (V[vx] == V[vy]) {
				PC += 4;
				break;
			}

			PC += 2;
			break;
		}
		case (0x6): // LD Vx , BYTE (SET Vx TO BYTE)
		{
			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t byte = opcode & 0xFF;      // EXTRACT BYTE VALUE
			V[vx] = byte;                      //SETS THE Vx REGISTER TO BYTE
			std::cout << "LD INSTRUCTION" << std::endl;
			break;
		}
		case(0x7): {//7xkk ADD Vx, BYTE
			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t byte = opcode & 0xFF;      // EXTRACT BYTE VALUE
			V[vx] = (V[vx] + byte) & 0xFF; // ENSURE WRAPPING IN ORDER TO AVOID GOING OVER 255

			std::cout << "ADD INSTRUCTION V" << (int)vx << ", " << (int)byte << std::endl;

			break;
		}
		case(0x8):
		{
			uint8_t nibble = (opcode & 0x0F);
			uint8_t x = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t y = (opcode >> 4) & 0x0F; // EXTRACT REGISTER NUMBER
			switch (nibble) {
				case(0x00): {
					V[x] = V[y];
					std::cout << "LD Vx, Vy" << std::endl;
					break;
				}
				case(0x01): {
					V[x] = V[x] | V[y];
					std::cout << "OR Vx, Vy" << std::endl;
					break;
				}
				case(0x02): {
					V[x] = V[x] & V[y];
					std::cout << "AND Vx, Vy" << std::endl;
					break;
				}
				case(0x03): {
					V[x] = V[x] ^ V[y];
					std::cout << "XOR Vx, Vy" << std::endl;
					break;
				}
				case(0x04): {
					uint16_t result = V[x] + V[y];
					V[0xF] = (result > 255) ? 0x01 : 0x00;
					V[x] = result & 0xFF;
					std::cout << "ADD Vx, Vy" << std::endl;
					break;
				}
				case(0x05): {
					V[0x0F] = (V[x] > V[y]) ? 0x01 : 0x00;
					V[x] -= V[y];
					std::cout << "SUB Vx, Vy" << std::endl;
					break;
				}
				case(0x06): {
					uint8_t lsb = V[x] & 1;

					V[0x0F] = (lsb == 1) ? 0x01 : 0x00;
					V[x] >>= 1;

					std::cout << "SHR Vx {, Vy}" << std::endl;
					break;
				}
				case(0x07): {
					V[0x0F] = (V[y] > V[x]) ? 0x01 : 0x00;
					V[x] = V[y] - V[x];
					std::cout << "SUBN Vx, Vy" << std::endl;
					break;
				}
				case(0x0E): {
					uint8_t msb = V[x] & 0x80;

					V[0x0F] = (msb == 0x80) ? 0x01 : 0x00;
					V[x] <<= 1;

					std::cout << "SHL Vx {, Vy}" << std::endl;
					break;
				}
			}
			break;
		}
		case(0x9): {
			if ((opcode & 0x00F) != 0x0) {
				std::cout << "INVALID OPCODE FORMAT" << std::endl;
				break;

			//0009 Vx00 Vy00 0000
			}
			uint8_t x = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t y = (opcode >> 4) & 0x0F; // EXTRACT REGISTER NUMBER

			if (V[x] != V[y]) {
				PC += 4;
			}
			break;
		}
		case(0xA): { //Annn SET VALUE OF I TO ADDRESS
			uint16_t address = opcode & 0x0FFF;
			I = address;
			std::cout << "Register I is set to " << I << std::endl;
			break;
		}
		case(0xD): //Dxyn - DRAW Vx, Vy, HEIGHT
		{
			uint8_t x = V[(opcode >> 8) & 0x0F]; // X-COORDINATE FROM VX (WRAPPING TO PREVENT FURTHER THAN 64PX)
			uint8_t y = V[(opcode >> 4) & 0x0F]; // Y-COORDINATE FROM VY (WRAPPING TO PREVENT FURTHER THAN 32PX)
			uint8_t height = opcode & 0x0F;      // HEIGHT OF SPRITE
			V[0xF] = 0;							 // Reset collision flag

			for (int row = 0; row < height; row++) {
				uint8_t sprite_byte = mem[I + row];
				for (int col = 0; col < 8; col++) {
					if ((sprite_byte & (0x80 >> col)) != 0) {
						int gfx_index = ((y + row) % 32) * 64 + ((x + col) % 64);
						if (gfx[gfx_index] == 1) {
							V[0xF] = 1; //COLLISION FLAG IS SET TO TRUE
						}
						gfx[gfx_index] ^= 1; //XOR OPERATOR
					}
				}
			}


			draw_flag = true; // SIGNAL TO REDRAW THE SCREEN
			break;
		}
		case(0x0): //0x0 INSTRUCTIONS
		{
			if ((opcode) == 0xE0) { //CLEAR SCREEN
				std::fill(std::begin(gfx), std::end(gfx), 0);
				std::cout << "CLEAR SCREEN CALLED" << std::endl;
				clearScreen_flag = true;
				break;
			}
			if ((opcode) == 0x00EE) { //RETURN FROM SUBROUTINE
				std::cout << "RETURN" << std::endl;
				PC = this->pop();
				std::cout << "THIS IS CURRENT PC " << PC << std::endl;
			}
			else {
				std::cout << "UNKNOWN OPCODE " << std::hex << opcode << std::endl;
			}
			break;
		}
		default:
			std::cout << "UNKNOWN OPCODE: " << std::hex << opcode << " at PC: " << PC << std::endl;
		}
	}


	void push(int16_t element) {
		if (SP > 16) {
			std::cerr << "STACK OVERFLOW" << std::endl;
			return;
		}
		stack[++SP] = element;
		std::cout << "WE HAVE PUSHED " << element << " ONTO THE STACK" << std::endl;

	}

	int16_t pop() {
		if (SP < 0) {
			return -1;
		}

		int16_t element = stack[SP];
		stack[SP] = 0;
		SP--;
		std::cout << "WE HAVE POPPED " << element << " OUT OF THE STACK" << std::endl;

		return element;
	}
};