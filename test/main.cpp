#include <iostream>

#include <cpp_playground/cpu.hpp>

int main()
{
    Computer computer;
    auto v = computer.memory.begin();
    *(v++) = 0x0104;
    *(v++) = 0x1005;
    *(v++) = 0xFE00;
    *(v++) = 0xFFFF;
    *(v++) = 0x0EE0;
    *(v++) = 0xB00F;

    while (!computer.isHalted())
    {
        computer.printState();
        computer.tick();
    }

    std::cout << "Output: " << std::dec << computer.out;
    std::cout << " 0x" << std::hex << computer.out << "\n";
}
