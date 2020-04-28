#include "cpp_playground/cpu.hpp"

#include <iomanip>
#include <iostream>
#include <optional>

namespace {
    HalfSize hi(ComputerBitness r) {
        return HalfSize((r & 0xFF00) >> 8);
    }

    HalfSize lo(ComputerBitness r) {
        return HalfSize((r & 0x00FF) >> 0);
    }

    std::optional<Flags> preamble(uint8_t p) {
        using namespace Flag;
        static std::array preamble = {
                Flags().set(COUNTER_OUT).set(MEM_ADDRESS_IN),
                Flags().set(COUNTER_INCREMENT).set(INSTRUCTION_IN).set(MEM_CELL_OUT),
        };
        return preamble[p];
    }

    std::optional<Flags> lookup(uint8_t p, Code instruction, AluFlags aluf) {
        using namespace Flag;
        switch (instruction) {
            case Code::LDA: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(MEM_ADDRESS_IN),
                                Flags().set(MEM_CELL_OUT).set(A_IN),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::LDI: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(A_IN),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::STA: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(MEM_ADDRESS_IN),
                                Flags().set(A_OUT).set(MEM_CELL_IN),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::ADD: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(MEM_ADDRESS_IN),
                                Flags().set(MEM_CELL_OUT).set(B_IN),
                                Flags().set(SUM_OUT).set(A_IN),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::SUB: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(MEM_ADDRESS_IN),
                                Flags().set(MEM_CELL_OUT).set(B_IN),
                                Flags().set(SUM_OUT).set(A_IN).set(SUM_SUBTRACT),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::JMP: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(JUMP),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::OUT: {
                static std::array tbl =
                        {
                                Flags().set(A_OUT).set(DISPLAY_IN),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::INT: {
                static std::array tbl =
                        {
                                Flags().set(INSTRUCTION_OUT).set(A_IN),
                                Flags().set(HALT),
                        };
                if (p < tbl.size())
                    return tbl[p];
                break;
            }
            case Code::JNZ: {
                if (aluf.test(AluFlag::ZF)) {
                    static std::array tbl =
                            {
                                    Flags().set(INSTRUCTION_OUT).set(JUMP),

                            };
                    if (p < tbl.size())
                        return tbl[p];
                }
                break;
            }
            case Code::NOP:
                break;
            default:;
        }
        return std::nullopt;
    }

    auto microCode(uint8_t p, Code instruction, AluFlags aluf) {
        return p < 2 ? preamble(p) : lookup(p - 2, instruction, aluf);
    }
}

bool Computer::isHalted() const {
    return flags.test(Flag::HALT) && a == 0;
}

void Computer::tick() {
    microTick();
    incrementIP();
    const auto bus = writeBusLine();
    readBusLine(bus);
}

void Computer::printState() const {
    std::cout << " MIP:" << std::hex << std::setfill('0') << std::setw(2) << int(mip);
    std::cout << " INST:" << std::hex << std::setfill('0') << std::setw(2) << int(hi(instruction));
    std::cout << " FLAG:" << flags;
    std::cout << " IP:" << std::hex << std::setfill('0') << std::setw(4) << ip;
    std::cout << " A:" << std::hex << std::setfill('0') << std::setw(4) << a;
    std::cout << " B:" << std::hex << std::setfill('0') << std::setw(4) << b;
    std::cout << " OUT:" << std::hex << std::setfill('0') << std::setw(4) << out;
    std::cout << " DATA:" << std::hex << std::setfill('0') << std::setw(4) << memory[address];
    std::cout << " ADDR:" << std::hex << std::setfill('0') << std::setw(4) << address;
    std::cout << "\n";
}

void Computer::incrementIP() {
    if (flags.test(Flag::COUNTER_INCREMENT)) {
        ++ip;
    }
}

Bus Computer::writeBusLine() {
    Bus bus = 0;
    if (flags.test(Flag::A_OUT)) {
        bus |= a;
    }
    if (flags.test(Flag::COUNTER_OUT)) {
        bus |= ip;
    }
    if (flags.test(Flag::SUM_OUT)) {
        bus |= flags.test(Flag::SUM_SUBTRACT) ? (a - b) : (a + b);
        alu.set(AluFlag::ZF,bus==0);
    }
    if (flags.test(Flag::MEM_CELL_OUT)) {
        bus |= memory[address];
    }
    if (flags.test(Flag::INSTRUCTION_OUT)) {
        bus |= lo(instruction);
    }
    return bus;
}

void Computer::readBusLine(Bus bus) {
    if (flags.test(Flag::JUMP)) {
        ip = bus;
    }
    if (flags.test(Flag::A_IN)) {
        a = bus;
    }
    if (flags.test(Flag::B_IN)) {
        b = bus;
    }
    if (flags.test(Flag::MEM_ADDRESS_IN)) {
        address = bus;
    }
    if (flags.test(Flag::INSTRUCTION_IN)) {
        instruction = bus;
    }
    if (flags.test(Flag::MEM_CELL_IN)) {
        memory[address] = bus;
    }
    if (flags.test(Flag::DISPLAY_IN)) {
        out = bus;
    }
}

void Computer::microTick() {
    if (const auto mc = microCode(mip, static_cast<Code>(hi(instruction)), alu)) {
        flags = *mc;
        ++mip;
    } else {
        flags = {};
        mip = 0;
        microTick();
    }
}

Interruptions Computer::checkInt() const {
    if (!flags.test(Flag::HALT))
        return Interruptions::NOINT;
    else {
        switch (a) {
            case 0x01: {
                return Interruptions::WAIT_INPUT;
            }
            case 0x02: {
                return Interruptions::WAIT_OUTPUT;
            }
            default: {
                return Interruptions::NOINT;
            }
        }
    }

}