#pragma once

#include <array>
#include <bitset>

using ComputerBitness = std::uint16_t;
using HalfSize = std::uint8_t;
static constexpr auto NUM_CELLS = std::numeric_limits<HalfSize>::max();

using Bus = ComputerBitness;
using Register = ComputerBitness;
using Memory = std::array<ComputerBitness, NUM_CELLS>;

namespace Flag
{
enum : std::size_t
{
    A_OUT,
    A_IN,
    DISPLAY_IN,
    B_IN,
    MEM_ADDRESS_IN,
    MEM_CELL_IN,
    MEM_CELL_OUT,
    SUM_OUT,
    SUM_SUBTRACT,
    INSTRUCTION_IN,
    INSTRUCTION_OUT,
    COUNTER_INCREMENT,
    COUNTER_OUT,
    JUMP,
    HALT,

    MAX
};
}

enum class Code : HalfSize
{
    NOP = 0x00,

    LDA = 0x01,
    LDI = 0x02,

    STA = 0x0a,

    ADD = 0x10,
    SUB = 0x12,

    JMP = 0x20,

    OUT = 0xFE,
    BRK = 0xFF,
};

using Flags = std::bitset<Flag::MAX>;

struct Computer final
{
    bool isHalted() const;

    void tick();

    void printState() const;

    // TODO: IO
    Memory memory = {};
    Register address = 0;
    Register out = 0;

private:
    Register a = 0;
    Register b = 0;
    Register ip = 0;
    Register mip = 0;
    Register instruction = 0;

    Flags flags = {};

    void incrementIP();
    Bus writeBusLine();
    void readBusLine(Bus bus);
    void microTick();
};
