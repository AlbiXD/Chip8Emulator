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
	uint8_t gbuffer[64 * 32];   // Created a buffer for the graphics 64 by 32
	Chip8() : I(0), PC(0x200), SP(0)
	{
		std::fill(std::begin(mem), std::end(mem), 0);
		std::fill(std::begin(V), std::end(V), 0);
		std::fill(std::begin(stack), std::end(stack), 0);
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
		case(0x1): {
			uint16_t address = opcode & 0x0FFF;
			PC = address;
			std::cout << "JUMP INSTRUCTION TO ADDRESS " << PC << std::endl;
			break;
		}
		case(0x2): {
			uint16_t address = opcode & 0x0FFF;
			std::cout << "CALLING SUBROUTINE TO " << address << std::endl;
			this->push(PC);
			PC = address;
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
		case(0x7): {
			uint8_t vx = (opcode >> 8) & 0x0F; // EXTRACT REGISTER NUMBER
			uint8_t byte = opcode & 0xFF;      // EXTRACT BYTE VALUE


			std::cout << "ADD INSTRUCTION V" << vx << ", " << byte << std::endl;

			break;
		}
		case(0xA): {
			uint16_t address = opcode & 0x0FFF;
			I = address;
			std::cout << "Register I is set to " << I << std::endl;
			break;
		}
		case(0xD):
		{

			std::cout << "DRAW FLAG" << std::endl;
			break;
		}
		default:
			std::cout << "UNKNOWN OPCODE" << std::endl;
		}
	}


	void push(int16_t element) {
		if (SP > 16) {
			std::cerr << "STACK OVERFLOW" << std::endl;
			return;
		}
		stack[SP++] = element;
	}

	int16_t pop() {
		if (SP < 0) {
			return -1;
		}

		int16_t element = stack[SP];
		stack[SP] = 0;
		SP--;
		return element;
	}
};