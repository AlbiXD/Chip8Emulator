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

    uint8_t V[16];     // 16 Registers with 8-bit (0-F in hexadecimal)
    uint16_t I;         // Registor to store memory addresses
    uint16_t PC;        // Stores currently executing address
    uint8_t SP;        // Stack pointer (Points at the top of the stack)
    uint16_t stack[16]; // Stack is an array of 16 16-bit values

    Chip8() : I(0), PC(0x200), SP(0)
    {
        std::fill(std::begin(mem), std::end(mem), 0);
        std::fill(std::begin(V), std::end(V), 0);
        std::fill(std::begin(stack), std::end(stack), 0);
    }

    /*
        TODO: Load instructions into memory
    */
    bool loadROM(const char *file_path)
    {
        std::ifstream file(file_path, std::ios::binary);

        if (!file)
        { // Check if file exists
            std::cout << "Incorrect File Name" << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end); // Moves pointer to the end of the file
        std::streampos fileSize = file.tellg(); // Gets the byte position at the end
        file.seekg(0, std::ios::beg); //Moves the pointer back to the beginning of the file in order to transfer bytes into memory

        if (fileSize > (4096 - 512)) // Checks if file is too large
        {
            std::cerr << "FILE SIZE TOO LARGE" << std::endl;
            return false;
        }

        if (!file.read(reinterpret_cast<char*>(mem + 512), fileSize)) { //Casts mem (int8_t*) to a char* and stores its values
            std::cerr << "Failed to read ROM into memory." << std::endl;
            return false;
        }

        file.close();
        return true;
    }

    void emulate_Cycle()
    {
        // fetch
        uint8_t byte1, byte2;
        uint16_t opcode;
        byte1 = mem[PC];
        byte2 = mem[PC+1];

        opcode = (byte1 << 8) | byte2;


        std::cout << std::hex << opcode << std::endl;


        //Starting position is 512

        // decode and execute
    }
};