#include "Chip8.cpp"
#include <iostream>
#include <fstream>
#include <iomanip> // For std::hex and std::setfill

int main()
{
    Chip8 chip;

    const char *fileName = "c8_test.c8";

    chip.loadROM(fileName);

    chip.emulate_Cycle();
    
    return 0;
}
