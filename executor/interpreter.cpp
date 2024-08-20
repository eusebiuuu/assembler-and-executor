#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <array>
#include <functional>
#include <cmath>

#include "../assembler/instruction_enum.h"
#include "../assembler/dumb_parser.hpp"
#include "../assembler/register_parser.hpp"
#include "../assembler/registers_enum.h"
#include "../assembler/instruction_size.hpp"

#define ll long long

using namespace std;

fstream file("../assembler/memory");
ifstream enc("../encoder/encodings.txt");
unordered_map<string, InstructionType> instr;

array<ll, INT_REGISTER_COUNT> int_register_value;
array<ll, FLOAT_REGISTER_COUNT> float_register_value;

int counter = 0;

const ll MAX_UINT = ((1LL << 32) - 1);

void find_encodings() {
	string curr_instr, curr_encoding;
	while (enc >> curr_instr >> curr_encoding) {
		instr[curr_encoding] = instruction_to_enum(curr_instr);
	}
    enc.close();
}

bitset<32> get_full_instruction(bitset<8> curr_byte, int cnt) {
    bitset<32> instruction_encoding = curr_byte.to_ulong();
    for (int _ = 1; _ < cnt; ++_) {
        char byte;
        file.read(&byte, 1);
        bitset<8> curr_byte = byte;
        const int pos = 8 * _;
        for (int i = pos; i < pos + 8; ++i) {
            instruction_encoding[i] = curr_byte[i - pos];
        }
    }
    return instruction_encoding;
}

int main() {
    find_encodings();
    int_register_value[RegisterIntType::sp] = TOTAL_MEMORY;
    bool end_of_program = false;
    while (!end_of_program) {
        file.seekg(counter);
        char byte;
        file.read(&byte, 1);
        bitset<8> curr_byte = byte;
        cout << curr_byte << '\n';
        string curr_encoding;
        int pos = 0;
        while (instr.find(curr_encoding) == instr.end()) {
            curr_encoding += curr_byte[pos++] + '0';
        }
        cout << curr_encoding << ' ' << instr[curr_encoding] << '\n';
        InstructionType curr_instruction = instr[curr_encoding];
        auto instruction_encoding = get_full_instruction(curr_byte, get_instruction_size(curr_instruction));

        auto get_number = [instruction_encoding, &pos](int sz, int sgn = 1) {
            int num = 0;
            for (int i = 0; i < sz - 1; ++i) {
                num += (1 << i) * instruction_encoding[pos++];
            }
            num += sgn * (1 << (sz - 1)) * instruction_encoding[pos++];
            return num;
        };

        counter += get_instruction_size(curr_instruction);

        switch (curr_instruction) {
            case InstructionType::add: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int reg3 = get_number(INT_REGISTER_BITS);
                int_register_value[reg1] = int_register_value[reg2] + int_register_value[reg3];
                break;
            }
            case InstructionType::li: {
                int reg = get_number(INT_REGISTER_BITS);
                int num = get_number(8, -1);
                int_register_value[reg] = num;
                break;
            }
            case InstructionType::addi: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int num = get_number(13, -1);
                int_register_value[reg1] = int_register_value[reg2] + num;
                break;
            }
            case InstructionType::mv: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int_register_value[reg1] = int_register_value[reg2];
                break;
            }
            case InstructionType::ret: {
                int const curr_address = MEMORY_SIZE - 3;
                int next_address = 0;
                file.seekg(curr_address);
                for (int i = 0; i < 4; ++i) {
                    char byte;
                    file.read(&byte, 1);
                    bitset<8> curr_byte = byte;
                    for (int j = 0; j < 8; ++j) {
                        next_address |= (1LL << (8 * i + j)) * curr_byte[j];
                    }
                }
                counter = next_address;

                if (next_address == 0) {
                    end_of_program = true;
                }
                
                file.seekp(curr_address);
                for (int i = 0; i < 4; ++i) {
                    file.put(0);
                }
                break;
            }
            case InstructionType::beqz: {
                int reg = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                if (int_register_value[reg] == 0) {
                    counter = address;
                }
                break;
            }
            case InstructionType::lb: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3, -1);
                int_register_value[reg1] = int_register_value[reg2] + offset;
                break;
            }
            case InstructionType::j: {
                int address = get_number(ADDRESS_SIZE);
                counter = address;
                break;
            }
            case InstructionType::sb: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3, -1);
                file.seekp(int_register_value[reg2] + offset);
                char value = int_register_value[reg1] & 255;
                file.put(value);
                break;
            }
            case InstructionType::bge: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                if (int_register_value[reg1] >= int_register_value[reg2]) {
                    counter = address;
                }
                break;
            }
            case InstructionType::sub: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int reg3 = get_number(INT_REGISTER_BITS);
                int_register_value[reg1] = int_register_value[reg2] - int_register_value[reg3];
                break;
            }
            case InstructionType::srai: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int num = get_number(2 + 8);
                int_register_value[reg1] = int_register_value[reg2] >> num;
                break;
            }
            case InstructionType::sd: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3 + 8, -1);
                ll start = int_register_value[reg2] + offset;
                ll num = int_register_value[reg1];
                for (int i = 0; i < 8; ++i) {
                    file.seekp(start + i);
                    file.put(num & 255);
                    num >>= 8;
                }
                break;
            }
            case InstructionType::call: {
                int address = get_number(ADDRESS_SIZE);
                if (address < TOTAL_MEMORY) {
                    int stack_address = int_register_value[RegisterIntType::sp];
                    file.seekp(stack_address - 3);
                    for (int i = 0; i < 4; ++i) {
                        char curr_address = stack_address & 255;
                        file.put(curr_address);
                        stack_address >>= 8;
                    }
                    counter = address;
                } else if (address == TOTAL_MEMORY + 1) {
                    // printf
                } else if (address == TOTAL_MEMORY + 2) {
                    // scanf
                } else {
                    cerr << "[ERROR]: Invalid address call\n";
                }
                break;
            }
            case InstructionType::ld: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3 + 8, -1);
                int_register_value[reg1] = offset + int_register_value[reg2];
                break;
            }
            case InstructionType::slli: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int num = get_number(2);
                int_register_value[reg1] = int_register_value[reg2] << num;
                break;
            }
            case InstructionType::lw: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3 + 8, -1);
                int_register_value[reg1] = offset + int_register_value[reg2];
                break;
            }
            case InstructionType::bnez: {
                int reg = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                if (int_register_value[reg] != 0) {
                    counter = address;
                }
                break;
            }
            case InstructionType::ble: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                if (int_register_value[reg1] <= int_register_value[reg2]) {
                    counter = address;
                }
                break;
            }
            case InstructionType::fld: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(4, -1);
                float_register_value[reg1] = offset + int_register_value[reg2];
                break;
            }
            case InstructionType::fsub_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] - float_register_value[reg3];
                break;
            }
            case InstructionType::fadd_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] + float_register_value[reg3];
                break;
            }
            case InstructionType::fmul_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] * float_register_value[reg3];
                break;
            }
            case InstructionType::fsqrt_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = sqrt(float_register_value[reg2]);
                break;
            }
            case InstructionType::fsw: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3, -1);
                ll start = int_register_value[reg2] + offset;
                ll num = float_register_value[reg1];
                for (int i = 0; i < 4; ++i) {
                    file.seekp(start + i);
                    file.put(num & 255);
                    num >>= 8;
                }
                break;
            }
            case InstructionType::fmv_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2];
                break;
            }
            case InstructionType::flt_s: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                int_register_value[reg1] = float_register_value[reg2] < float_register_value[reg3];
                break;
            }
            case InstructionType::fgt_s: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                int_register_value[reg1] = float_register_value[reg2] > float_register_value[reg3];
                break;
            }
            case InstructionType::bgt: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                if (int_register_value[reg1] > int_register_value[reg2]) {
                    counter = address;
                }
                break;
            }
            case InstructionType::fmv_s_x: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] & MAX_UINT;
                break;
            }
            case InstructionType::fmul_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = (float_register_value[reg2] * float_register_value[reg3]) & MAX_UINT;
            }
            case InstructionType::fadd_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = (float_register_value[reg2] + float_register_value[reg3]) & MAX_UINT;
            }
            case InstructionType::flw: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3, -1);
                float_register_value[reg1] = offset + int_register_value[reg2];
                break;
            }
            case InstructionType::la: {
                int reg = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                int_register_value[reg] = address;
                break;
            }
        }
        break;
    }
    return 0;
}
