#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <array>
#include <functional>
#include <cmath>
#include <cstring>
#include <iomanip>

#include "../utils/instruction_enum.hpp"
#include "../utils/dumb_parser.hpp"
#include "../utils/register_parser.hpp"
#include "../utils/registers_enum.hpp"
#include "../utils/instruction_size.hpp"
#include "../utils/cpu_spec.hpp"

std::fstream file;
std::ofstream fout;
std::ifstream enc;
std::ifstream int_reg;
std::ifstream float_reg;

std::unordered_map<std::string, InstructionType> instr;

std::array<std::string, INT_REGISTER_COUNT> int_register_name;
std::array<std::string, FLOAT_REGISTER_COUNT> float_register_name;

std::array<uint64_t, INT_REGISTER_COUNT> int_register_value;
std::array<uint64_t, FLOAT_REGISTER_COUNT> float_register_value;

int counter = 0;

const uint64_t MAX_UINT = ((1LL << 32) - 1);

void find_instructions() {
	std::string curr_instr, curr_encoding;
	while (enc >> curr_instr >> curr_encoding) {
		instr[curr_encoding] = instruction_to_enum(curr_instr);
	}
}

void get_int_register_names() {
    std::string reg;
    while (int_reg >> reg) {
        int_register_name[(int)parseIntRegister(reg)] = reg;
    }
}

void get_float_register_names() {
    std::string reg;
    while (float_reg >> reg) {
        float_register_name[(int)parseFloatRegister(reg)] = reg;
    }
}

std::bitset<32> get_full_instruction(std::bitset<8> curr_byte, int cnt) {
    std::bitset<32> instruction_encoding = curr_byte.to_ulong();
    int pos = 8;
    for (int _ = 1; _ < cnt; ++_) {
        char byte;
        file.seekg(counter + _);
        file.read(&byte, 1);
        std::bitset<8> curr_byte = byte;
        for (int i = 0; i < 8; ++i) {
            instruction_encoding[pos++] = curr_byte[i];
        }
    }
    return instruction_encoding;
}

std::string get_string_variable(int address) {
    file.seekg(address);
    char byte;
    std::string ans;

    file.read(&byte, 1);
    while (byte != 0) {
        ans += byte;
        file.read(&byte, 1);
    }
    return ans;
}

void store_number_at_address(uint64_t num, uint64_t address, int bytes) {
    for (int i = 0; i < bytes; ++i) {
        file.seekp(address);
        unsigned char curr_num = num & 255;
        file.put(curr_num);
        num >>= 8;
        address++;
    }
}

void store_string_at_address(std::string str, uint32_t address) {
    str += '\0';
    int sz = str.size();
    for (int i = 0; i < sz; ++i) {
        file.seekp(address);
        file.put(str[i]);
        address++;
    }
}

void load_number_from_address(uint64_t &dest, uint64_t address, int bytes) {
    dest = 0;
    for (int i = 0; i < bytes; ++i) {
        file.seekg(address);
        char byte;
        file.read(&byte, 1);
        std::bitset<8> curr_byte = byte;
        for (int j = 0; j < 8; ++j) {
            dest |= (1ULL << ((8 * i) + j)) * curr_byte[j];
        }
        address++;
    }
}

float convert_to_float(uint64_t num) {
    uint32_t good_num = num & MAX_UINT;
    float ans;
    memcpy(&ans, &good_num, sizeof(good_num));
    return ans;
}

double convert_to_double(uint64_t num) {
    double ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

uint32_t convert_to_int(float num) {
    uint32_t ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

uint64_t convert_to_long_long(double num) {
    uint64_t ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

int main(int argc, char* argv[]) {
    file.open(argv[1]);
    fout.open(argv[2]);
    enc.open(argv[3]);
    int_reg.open(argv[4]);
    float_reg.open(argv[5]);

    find_instructions();

    std::cout << "Start interpreting the binary...\n";

    int_register_value[(int)RegisterIntType::sp] = TOTAL_MEMORY - STACK_STEP + 1;
    int_register_value[(int)RegisterIntType::ra] = EXIT_ADDRESS;

    bool exit_program = false;

    // int limit = 100;

    while (!exit_program) {
        // limit--;
        // if (limit == 0) {
        //     break;
        // }
        if (counter == EXIT_ADDRESS) {
            exit_program = true;
            break;
        }
        file.seekg(counter);
        char byte;
        file.read(&byte, 1);
        std::bitset<8> curr_byte = byte;
        std::string curr_encoding;
        int pos = 0;
        while (instr.find(curr_encoding) == instr.end()) {
            curr_encoding += curr_byte[pos++] + '0';
        }
        //std::cout << "Instruction code: " << (int)instr[curr_encoding] << '\n';
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
                counter = int_register_value[(int)RegisterIntType::ra];
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
                load_number_from_address(int_register_value[reg1], int_register_value[reg2] + offset, 1);
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
                store_number_at_address(int_register_value[reg1], int_register_value[reg2] + offset, 1);
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
                store_number_at_address(int_register_value[reg1], int_register_value[reg2] + offset, 8);
                break;
            }
            case InstructionType::call: {
                int address = get_number(ADDRESS_SIZE);
                if (address < TOTAL_MEMORY) {
                    int_register_value[(int)RegisterIntType::ra] = counter;
                    counter = address;
                } else if (address == TOTAL_MEMORY + 1)  {
                    std::string format_string = get_string_variable(int_register_value[(int)RegisterIntType::a0]);
                    std::string::size_type pos = 0;
                    bool print_string = format_string.find("%s", 0) != std::string::npos;
                    bool print_int = format_string.find("%d", 0) != std::string::npos;
                    bool print_long_long = format_string.find("%lld", 0) != std::string::npos;
                    bool print_double = format_string.find("%lf", 0) != std::string::npos;
                    bool print_float = format_string.find("%f", 0) != std::string::npos;
                    bool all_false = !print_string && !print_int && !print_double && !print_long_long && !print_float;
                    if (all_false) {
                        printf("%s", format_string.c_str());
                    } else if (print_string) {
                        std::string string_to_print = get_string_variable(int_register_value[(int)RegisterIntType::a1]);
                        printf(format_string.c_str(), string_to_print.c_str());
                    } else if (print_int) {
                        printf(format_string.c_str(), int_register_value[(int)RegisterIntType::a1]);
                    } else if (print_long_long) {
                        printf(format_string.c_str(), int_register_value[(int)RegisterIntType::a1]);
                    } else if (print_double) {
                        printf(format_string.c_str(), convert_to_double(int_register_value[(int)RegisterIntType::a1]));
                    } else if (print_float) {
                        printf(format_string.c_str(), convert_to_float(int_register_value[(int)RegisterIntType::a1]));
                    } else {
                        std::cerr << "[ERROR]: Due to the limitations of the project the interpreter cannot execute your instruction\n";
                    }
                } else if (address == TOTAL_MEMORY + 2) {
                    std::string format_string = get_string_variable(int_register_value[(int)RegisterIntType::a0]);
                    std::string::size_type pos = 0;
                    bool read_string = format_string.find("%s", 0) != std::string::npos;
                    bool read_int = format_string.find("%d", 0) != std::string::npos;
                    bool read_long_long = format_string.find("%lld", 0) != std::string::npos;
                    bool read_double = format_string.find("%lf", 0) != std::string::npos;
                    bool read_float = format_string.find("%f", 0) != std::string::npos;
                    if (read_string) {
                        char input[MAX_STRING_LEN];
                        scanf(format_string.c_str(), input);
                        std::string str_input(input);
                        store_string_at_address(input, int_register_value[(int)RegisterIntType::a1]);
                    } else if (read_int) {
                        int num;
                        scanf(format_string.c_str(), &num);
                        store_number_at_address(num, int_register_value[(int)RegisterIntType::a1], 4);
                    } else if (read_long_long) {
                        uint64_t num;
                        scanf(format_string.c_str(), &num);
                        store_number_at_address(num, int_register_value[(int)RegisterIntType::a1], 8);
                    } else if (read_double) {
                        double num;
                        scanf(format_string.c_str(), &num);
                        uint64_t number_to_store = convert_to_long_long(num);
                        store_number_at_address(number_to_store, int_register_value[(int)RegisterIntType::a1], 8);
                    } else if (read_float) {
                        float num;
                        scanf(format_string.c_str(), &num);
                        uint64_t number_to_store = convert_to_int(num);
                        store_number_at_address(number_to_store, int_register_value[(int)RegisterIntType::a1], 4);
                    } else {
                        std::cerr << "[ERROR]: Due to the limitations of the project the interpreter cannot execute your instruction\n";
                    }
                } else if (address == TOTAL_MEMORY + 3) {
                    std::string str = get_string_variable(int_register_value[(int)RegisterIntType::a0]);
                    int_register_value[(int)RegisterIntType::a0] = str.length();
                } else {
                    std::cerr << "[ERROR]: Invalid function call\n";
                }
                break;
            }
            case InstructionType::ld: {
                int reg1 = get_number(INT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3 + 8, -1);
                load_number_from_address(int_register_value[reg1], offset + int_register_value[reg2], 8);
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
                load_number_from_address(int_register_value[reg1], offset + int_register_value[reg2], 4);
                // cout << "Load value: " << int_register_value[reg1] << '\n';
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
                int offset = get_number(4);
                load_number_from_address(float_register_value[reg1], offset + int_register_value[reg2], 8);
                break;
            }
            case InstructionType::fsub_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = 
                    convert_to_long_long(convert_to_double(float_register_value[reg2]) - convert_to_double(float_register_value[reg3]));
                break;
            }
            case InstructionType::fadd_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = 
                    convert_to_long_long(convert_to_double(float_register_value[reg2]) + convert_to_double(float_register_value[reg3]));
                break;
            }
            case InstructionType::fmul_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = 
                    convert_to_long_long(convert_to_double(float_register_value[reg2]) * convert_to_double(float_register_value[reg3]));
                break;
            }
            case InstructionType::fsqrt_d: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = convert_to_long_long(sqrt(convert_to_double(float_register_value[reg2])));
                break;
            }
            case InstructionType::fsw: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3);
                store_number_at_address(float_register_value[reg1], int_register_value[reg2] + offset, 4);
                break;
            }
            case InstructionType::fmv_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] & MAX_UINT;
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
                int reg2 = get_number(INT_REGISTER_BITS);
                float_register_value[reg1] = convert_to_int(convert_to_float(int_register_value[reg2]));
                break;
            }
            //case InstructionType::fmul_s: {
            //    int reg1 = get_number(FLOAT_REGISTER_BITS);
            //    int reg2 = get_number(FLOAT_REGISTER_BITS);
            //    int reg3 = get_number(FLOAT_REGISTER_BITS);
            //    float_register_value[reg1] = 
            //        convert_to_int(convert_to_float(float_register_value[reg2]) * convert_to_float(float_register_value[reg3]));
            //}
            //case InstructionType::fadd_s: {
            //    int reg1 = get_number(FLOAT_REGISTER_BITS);
            //    int reg2 = get_number(FLOAT_REGISTER_BITS);
            //    int reg3 = get_number(FLOAT_REGISTER_BITS);
            //    float_register_value[reg1] = 
            //        convert_to_int(convert_to_float(float_register_value[reg2]) + convert_to_float(float_register_value[reg3]));
            //}
            //case InstructionType::flw: {
            //    int reg1 = get_number(FLOAT_REGISTER_BITS);
            //    int reg2 = get_number(INT_REGISTER_BITS);
            //    int offset = get_number(3, -1);
            //    load_number_from_address(float_register_value[reg1], offset + int_register_value[reg2], 4);
            //    break;
            //}
            case InstructionType::la: {
                int reg = get_number(INT_REGISTER_BITS);
                int address = get_number(ADDRESS_SIZE);
                int_register_value[reg] = address;
                // cout << "Load address la: " << address << '\n';
                break;
            }
        }
    }
    std::cout << "Binary interpreting finished!\n";

    std::cout << "Saving the program data to state file...\n";
    get_int_register_names();
    get_float_register_names();
    fout << "INT registers: \n";
    for (int i = 0; i < INT_REGISTER_COUNT; ++i) {
        fout << int_register_name[i] << ": " << int_register_value[i] << '\n';
    }
    fout << "\nFLOAT registers: \n";
    for (int i = 0; i < FLOAT_REGISTER_COUNT; ++i) {
        fout << float_register_name[i] << ": " << float_register_value[i] << '\n';
    }
    fout << "\nSTACK: \n";
    int stack_pointer = int_register_value[(int)RegisterIntType::sp];
    while (stack_pointer >= MEMORY_SIZE + BINARY_SIZE) {
        uint64_t curr_num = 0;
        load_number_from_address(curr_num, stack_pointer, STACK_STEP);
        fout << "0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << stack_pointer << ": ";
        fout << "0x" << std::uppercase << std::setw(16) << std::setfill('0') << std::hex << curr_num << '\n';
        stack_pointer -= STACK_STEP;
    }
    std::cout << "Saving program data finished\n";
    return 0;
}
