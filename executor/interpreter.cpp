#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <array>
#include <functional>
#include <cmath>
#include <cstring>

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
    int pos = 0;
    for (int _ = 1; _ < cnt; ++_) {
        char byte;
        file.read(&byte, 1);
        bitset<8> curr_byte = byte;
        for (int i = 0; i < 8; ++i) {
            instruction_encoding[pos++] = curr_byte[i];
        }
    }
    return instruction_encoding;
}

string get_string_variable(int address) {
    file.seekg(address);
    char byte;
    file.read(&byte, 1);
    int len = (int) byte;
    address++;

    string ans;
    for (int i = 0; i < len; ++i) {
        file.seekg(address + i);
        file.read(&byte, 1);
        ans += byte;
    }
    return ans;
}

int get_format_func_params(string s) {
    string formats[] = {"%ld", "%d", "%hu", "%s"};
    int total_count = 0;
    for (auto str : formats) {
        string::size_type pos = 0;
        while ((pos = s.find(str, pos)) != string::npos) {
            total_count++;
            pos += str.length();
        }
    }
    return total_count;
}

void store_number_at_address(ll num, ll address, int bytes) {
    for (int i = 0; i < bytes; ++i) {
        file.seekp(address);
        unsigned char curr_num = num & 255;
        file.put(curr_num);
        num >>= 8;
        address++;
    }
}

void load_number_from_address(ll &dest, ll address, int bytes) {
    dest = 0;
    for (int i = 0; i < bytes; ++i) {
        file.seekg(address);
        char byte;
        file.read(&byte, 1);
        bitset<8> curr_byte = byte;
        for (int j = 0; j < 8; ++j) {
            dest |= (1 << (8 * i) + j);
        }
        address++;
    }
}

float convert_to_float(ll num) {
    uint32_t good_num = num & MAX_UINT;
    float ans;
    memcpy(&ans, &good_num, sizeof(good_num));
    return ans;
}

double convert_to_double(ll num) {
    double ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

int convert_to_int(float num) {
    int ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

ll convert_to_long_long(double num) {
    ll ans;
    memcpy(&ans, &num, sizeof(num));
    return ans;
}

int main() {
    find_encodings();

    int_register_value[RegisterIntType::sp] = TOTAL_MEMORY - STACK_STEP + 1;
    int_register_value[RegisterIntType::ra] = EXIT_ADDRESS;

    bool exit_program = false;

    while (!exit_program) {
        if (counter == EXIT_ADDRESS) {
            exit_program = true;
            break;
        }
        file.seekg(counter);
        char byte;
        file.read(&byte, 1);
        bitset<8> curr_byte = byte;
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
                counter = int_register_value[RegisterIntType::ra];
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
                    int_register_value[RegisterIntType::ra] = counter;
                    counter = address;
                } else if (address == TOTAL_MEMORY + 1)  {
                    string aux = get_string_variable(int_register_value[RegisterIntType::a0]);
                    int params = get_format_func_params(aux);
                    const char *format_string = aux.c_str();
                    cout << format_string << ' ' << params << '\n';
                    switch (params) {
                        case 0: {
                            printf("%s", format_string);
                            break;
                        }
                        case 1: {
                            printf(format_string, int_register_value[RegisterIntType::a1]);
                            break;
                        }
                        case 2: {
                            printf(format_string, int_register_value[RegisterIntType::a1], int_register_value[RegisterIntType::a2]);
                            break;
                        }
                        case 3: {
                            printf(format_string, int_register_value[RegisterIntType::a1], int_register_value[RegisterIntType::a2], int_register_value[RegisterIntType::a3]);
                            break;
                        }
                        default: {
                            cerr << "[ERROR]: Printf has too many params to handle\n";
                            break;
                        }
                    }
                } else if (address == TOTAL_MEMORY + 2) {
                    string aux = get_string_variable(int_register_value[RegisterIntType::a0]);
                    int params = get_format_func_params(aux);
                    const char *format_string = aux.c_str();
                    cout << format_string << ' ' << params << '\n';
                    int const MAX_PARAMS = 3;
                    int arr[MAX_PARAMS];
                    switch (params) {
                        case 1: {
                            scanf(format_string, &arr[0]);
                            store_number_at_address(arr[0], int_register_value[RegisterIntType::a1], STACK_STEP);
                            break;
                        }
                        case 2: {
                            scanf(format_string, &arr[0], &arr[1]);
                            store_number_at_address(arr[0], int_register_value[RegisterIntType::a1], STACK_STEP);
                            store_number_at_address(arr[1], int_register_value[RegisterIntType::a2], STACK_STEP);
                            break;
                        }
                        case 3: {
                            scanf(format_string, &arr[0], &arr[1], &arr[2]);
                            store_number_at_address(arr[0], int_register_value[RegisterIntType::a1], STACK_STEP);
                            store_number_at_address(arr[1], int_register_value[RegisterIntType::a2], STACK_STEP);
                            store_number_at_address(arr[2], int_register_value[RegisterIntType::a3], STACK_STEP);
                            break;
                        }
                        default: {
                            cerr << "[ERROR]: Scanf has too many params to handle\n";
                            break;
                        }
                    }
                } else if (address == TOTAL_MEMORY + 3) {
                    string str = get_string_variable(int_register_value[RegisterIntType::s1]);
                    int_register_value[RegisterIntType::a0] = str.length();
                } else {
                    cerr << "[ERROR]: Invalid function call\n";
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
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = float_register_value[reg2] & MAX_UINT;
                break;
            }
            case InstructionType::fmul_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = 
                    convert_to_int(convert_to_float(float_register_value[reg2]) * convert_to_float(float_register_value[reg3]));
            }
            case InstructionType::fadd_s: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(FLOAT_REGISTER_BITS);
                int reg3 = get_number(FLOAT_REGISTER_BITS);
                float_register_value[reg1] = 
                    convert_to_int(convert_to_float(float_register_value[reg2]) + convert_to_float(float_register_value[reg3]));
            }
            case InstructionType::flw: {
                int reg1 = get_number(FLOAT_REGISTER_BITS);
                int reg2 = get_number(INT_REGISTER_BITS);
                int offset = get_number(3, -1);
                load_number_from_address(float_register_value[reg1], offset + int_register_value[reg2], 4);
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
