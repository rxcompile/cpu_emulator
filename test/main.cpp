#include <iostream>

#include <calculator/cpu.hpp>
#include <iomanip>

int main()
{
    Computer computer;
    auto v = computer.memory.begin();
    *(v++) = 0x0200;
    *(v++) = 0x0aF0;
    *(v++) = 0xFF01;
    *(v++) = 0x01CF;
    *(v++) = 0x0aF1;
    *(v++) = 0xFF01;
    *(v++) = 0x01CF;
    *(v++) = 0x0aF2;
    *(v++) = 0xFF01;
    *(v++) = 0x01CF;
    *(v++) = 0x10F0;
    *(v++) = 0x2111;
    *(v++) = 0x01F1;
    *(v++) = 0x12F2;
    *(v++) = 0xFE00;
    *(v++) = 0xFF02;
    *(v++) = 0x2000;
    *(v++) = 0x01F1;
    *(v++) = 0x10F2;
    *(v++) = 0xFE00;
    *(v++) = 0xFF02;
    *(v++) = 0x2000;

    while (!computer.isHalted())
    {
        switch (computer.checkInt())
        {
            case Interruptions::WAIT_INPUT :
            {
                uint16_t in;
                std::cin >> in;
                computer.memory[0xCF] = in;
                break;
            }
            case Interruptions::WAIT_OUTPUT:
            {
                std::cout << "Output: " << std::dec << computer.out;
                std::cout << " 0x" << std::hex << computer.out << "\n";
                std::cout <<std::endl;
                break;
            }
        }
        computer.tick();
    }

}
