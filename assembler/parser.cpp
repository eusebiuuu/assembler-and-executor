#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include <unordered_map>
#include <bitset>

#include "instruction_enum.h"
#include "dumb_parser.hpp"
#include "register_parser.hpp"
#include "registers_enum.h"
#include "instruction_size.hpp"

using namespace std;
ofstream fout("memory");

ifstream fin("../inputs/01_String_Length/asm.s");
ifstream enc("../encoder/encodings.txt");
unordered_map<InstructionType, string> encodings;
vector<pair<string, int>> label_address;
vector<pair<string, string>> variables_list;
unordered_map<string, int> variable_address;

int current_bit_offset = 0, current_address = 0;

string main_label;

void join_bit(bool bit) {
	static std::bitset<8> current_byte;
	current_byte[current_bit_offset] = bit;
	current_bit_offset++;
	if (current_bit_offset >= 8) {
		fout << (unsigned char)current_byte.to_ulong();
		current_address++;
		current_bit_offset = 0;
	}
}

void pad_current_byte() {
	while (current_bit_offset) {
		join_bit(0);
	}
}

void pad_bytes(int sz) {
	while (current_address < sz) {
		join_bit(0);
	}
}

void emit_instruction(InstructionType instruction) {
	pad_current_byte();
	for (char c : encodings[instruction]) {
		join_bit(c - '0');
	}
}

void emit_immediate(int num, const int bits_count) {
	if (num < 0) {
		num = (1 << bits_count) - abs(num);
	}
	for (int i = 0; i < bits_count; ++i) {
		bool bit = num & (1 << i);
		join_bit(bit);
	}
}

void emit_register(RegisterIntType reg) {
	emit_immediate(reg, 4);
}

void emit_register(RegisterFloatType reg) {
	emit_immediate(reg, 3);
}

void emit_variables() {
	for (auto elem : variables_list) {
		variable_address[elem.first] = current_address;

		emit_immediate(elem.second.size(), 8);
		for (char c : elem.second) {
			emit_immediate(c, 8);
		}
	}
}



void jump_over_spaces(int &pos, string line) {
	while (pos < (int) line.size() && isspace(line[pos])) {
		pos++;
	}
}

string get_word(int &pos, string line) {
	string word;
	while (pos < (int) line.size() && (isalnum(line[pos]) || line[pos] == '_' || line[pos] == '.')) {
		word += line[pos++];
	}
	return word;
}

bool is_directive(string line) {
	int pos = 0;
	jump_over_spaces(pos, line);
	if (pos >= (int) line.size() || line[pos] != '.') {
		return false;
	}
	return true;
}

vector<string> get_directive(string line) {
	vector<string> info;
	int pos = line.find('.') + 1;
	info.push_back(get_word(pos, line));
	jump_over_spaces(pos, line);

	pos += info[0] == "section";

	info.push_back(get_word(pos, line));
	cout << "Directive: " << info[0] << ' ' << info[1] << '\n';
	return info;
}

bool is_label(string line) {
	int pos = 0;
	jump_over_spaces(pos, line);
	if (pos >= (int) line.size()) {
		return false;
	}
	while (pos < (int) line.size() && (isalnum(line[pos]) || line[pos] == '_')) {
		pos++;
	}
	if (pos >= (int) line.size() || line[pos] != ':') {
		return false;
	}
	return true;
}

string get_label(string line) {
	int pos = 0;
	jump_over_spaces(pos, line);
	string aux = get_word(pos, line);
	cout << "Label: " << aux << '\n';
	return aux;
}

void parse_rodata() {
	string line;
	while (getline(fin, line)) {
		if (!is_label(line)) break;

		string label = get_label(line);
		int pos = line.find(":") + 1;
		jump_over_spaces(pos, line);
		get_word(pos, line);
		jump_over_spaces(pos, line);
		pos++;
		int ending_pos = line.find("\"", pos);
		string str = line.substr(pos, ending_pos - pos);

		cout << "Variable: " << label << ' ' << str << str.size() << '\n';
        variables_list.push_back({label, str});
	}
}

void find_labels_addresses() {
	string line;
    while (getline(fin, line)) {
		if (is_directive(line)) {
			auto info = get_directive(line);
			if (info[0] == "global") {
				main_label = info[1];
			} else if (info[1] == "rodata") {
				parse_rodata();
			}
		} else if (is_label(line)) {
			string label = get_label(line);
			label_address.push_back({label, current_address});
		} else {
			int pos = 0;
			jump_over_spaces(pos, line);
			if (pos >= (int) line.size() || line[pos] == '#') {
				continue;
			}

			string instruction = get_word(pos, line);
            current_address += get_instruction_size(instruction_to_enum(instruction));
        }
    }
    fin.clear();
    fin.seekg(0, fin.beg);
    current_address = 0;
}

int get_closest_address(string label, int address, int dir) {
    int ans = -1;
    if (dir == -1) {
        for (auto elem : label_address) {
            if (elem.second < address and elem.first == label) {
                ans = elem.second;
            }
        }
    } else if (dir == 1) {
        for (auto elem : label_address) {
            if (elem.second > address and elem.first == label) {
                ans = elem.second;
                break;
            }
        }
    }
    return ans;
}

int get_label_address(string label) {
	for (auto elem : label_address) {
		if (elem.first == label) {
			return elem.second;
		}
	}
	return -1;
}

void find_encodings() {
	string instr, currEnc;
	while (enc >> instr >> currEnc) {
		encodings[instruction_to_enum(instr)] = currEnc;
	}
}

int main() {
	find_encodings();
	current_address = MEMORY_SIZE;
	find_labels_addresses();

	for (auto elem : label_address) {
		cout << elem.first << ' ' << elem.second << '\n';
	}
	
	int main_address = get_label_address(main_label);
	emit_instruction(InstructionType::j);
	emit_immediate(main_address, ADDRESS_SIZE);
	cout << main_address << '\n';
	pad_current_byte();

	emit_variables();

	pad_bytes(MEMORY_SIZE);

    string line;
	while (getline(fin, line)) {
		if (is_directive(line) || is_label(line)) {
			continue;
		}
		int pos = 0;
		jump_over_spaces(pos, line);
		if (pos >= (int) line.size() || line[pos] == '#') {
			continue;
		}

		string instruction = get_word(pos, line);
		cout << "Instruction: " << instruction << '\n';

		auto captureNumber = [&pos, line]() -> int {
			int ans = 0, sgn = 1;
			if (line[pos] == '-') {
				sgn = -1;
				pos++;
			}
			while (pos < (int) line.size() && '0' <= line[pos] && line[pos] <= '9') {
				ans = ans * 10 + (line[pos++] - '0');
			}
			ans *= sgn;
			return ans;
		};

		auto captureWord = [&pos, line]() -> string {
			string word;
			while (pos < (int) line.size() && (isalnum(line[pos]) || line[pos] == '_')) {
				word += line[pos++];
			}
			return word;
		};

		auto jumpOverNonAlNum = [&pos, line]() -> void {
			while (pos < (int) line.size() && (isspace(line[pos]) || line[pos] == ',')) {
				pos++;
			}
		};

		auto captureLabel = [&pos, line]() -> string {
			string label;
			while (pos < (int) line.size() && isalnum(line[pos])) {
				label += line[pos++];
			}
			return label;
		};

		auto getRegisters = [captureWord, jumpOverNonAlNum](const int cnt) -> vector<string> {
			vector<string> registers;
			for (int _ = 0; _ < cnt; ++_) {
				jumpOverNonAlNum();
				registers.push_back(captureWord());
			}
			return registers;
		};

		auto get2RegistersOffset = [captureNumber, captureWord, &pos, jumpOverNonAlNum]() -> pair<array<string, 2>, int> {
			array<string, 2> regs;
			jumpOverNonAlNum();
			regs[0] = captureWord();
			jumpOverNonAlNum();
			int offset = captureNumber();
			pos++;
			regs[1] = captureWord();
			return {regs, offset};
		};

		InstructionType curr_type = instruction_to_enum(instruction);

		switch (curr_type) {
			case InstructionType::add: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::add);
				for (auto elem : registers) {
					emit_register(parseIntRegister(elem));
				}
				break;
			}
			case InstructionType::li: {
				string currRegister;
				jumpOverNonAlNum();
				currRegister = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				emit_instruction(InstructionType::li);
				emit_register(parseIntRegister(currRegister));
				emit_immediate(num, 8);
				break;
			}
			case InstructionType::addi: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				emit_instruction(InstructionType::addi);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 13);
				break;
			}
			case InstructionType::mv: {
				auto registers = getRegisters(2);

				emit_instruction(InstructionType::mv);
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseIntRegister(registers[1]));
				break;
			}
			case InstructionType::ret: {
				emit_instruction(InstructionType::ret);
				break;
			}
			case InstructionType::beqz: {
				jumpOverNonAlNum();
				string reg = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				emit_instruction(InstructionType::beqz);
				emit_register(parseIntRegister(reg));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::lb: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;

				emit_instruction(InstructionType::lb);
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::j: {
				jumpOverNonAlNum();
				string label = captureLabel();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				emit_instruction(InstructionType::j);
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::sb: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;

				emit_instruction(InstructionType::sb);
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::bge: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				emit_instruction(InstructionType::bge);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::sub: {
				auto registers = getRegisters(3);

				emit_instruction(InstructionType::sub);
				for (auto elem : registers) {
					emit_register(parseIntRegister(elem));
				}
				break;
			}
			case InstructionType::srai: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				emit_instruction(InstructionType::srai);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 2 + 8);
				break;
			}
			case InstructionType::sd: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::sd);
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::call: {
				jumpOverNonAlNum();
				string label = captureLabel();
				emit_instruction(InstructionType::call);
				int address = get_label_address(label);
				if (address != -1) {
					emit_immediate(address, ADDRESS_SIZE);
				} else {
					if (label == "printf") {
						emit_immediate(TOTAL_MEMORY + 1, ADDRESS_SIZE);
					} else if (label == "scanf") {
						emit_immediate(TOTAL_MEMORY + 2, ADDRESS_SIZE);
					} else if (label == "strlen") {
						emit_immediate(TOTAL_MEMORY + 3, ADDRESS_SIZE);
					} else {
						emit_immediate(INVALID_FUNCTION_CALL, ADDRESS_SIZE);
					}
				}
				break;
			}
			case InstructionType::ld: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::ld);
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::slli: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();
				emit_instruction(InstructionType::slli);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 2);
				break;
			}
			case InstructionType::lw: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::lw);
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::bnez: {
				jumpOverNonAlNum();
				string reg = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				emit_instruction(InstructionType::bnez);
				emit_register(parseIntRegister(reg));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::ble: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				emit_instruction(InstructionType::ble);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::fld: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::fld);
				emit_register(parseFloatRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 4);
				break;
			}
			case InstructionType::fsub_d: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fsub_d);
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fadd_d: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fadd_d);
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fmul_d: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fmul_d);
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fsqrt_d: {
				auto registers = getRegisters(2);
				emit_instruction(InstructionType::fsqrt_d);
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				break;
			}
			case InstructionType::fsw: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::fsw);
				emit_register(parseFloatRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::fmv_s: {
				auto registers = getRegisters(2);
				emit_instruction(InstructionType::fmv_s);
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				break;
			}
			case InstructionType::flt_s: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::flt_s);
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				emit_register(parseFloatRegister(registers[2]));
				break;
			}
			case InstructionType::fgt_s: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fgt_s);
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				emit_register(parseFloatRegister(registers[2]));
				break;
			}
			case InstructionType::bgt: {
				jumpOverNonAlNum();
				string reg1 = captureWord();
				jumpOverNonAlNum();
				string reg2 = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				emit_instruction(InstructionType::bgt);
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::fmv_s_x: {
				auto registers = getRegisters(2);
				emit_instruction(InstructionType::fmv_s_x);
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				break;
			}
			case InstructionType::fmul_s: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fmul_s);
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fadd_s: {
				auto registers = getRegisters(3);
				emit_instruction(InstructionType::fadd_s);
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::flw: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				emit_instruction(InstructionType::flw);
				emit_register(parseFloatRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::la: {
				jumpOverNonAlNum();
				string reg = captureWord();
				jumpOverNonAlNum();
				string label = captureWord();
				emit_instruction(InstructionType::la);
				emit_register(parseIntRegister(reg));
				emit_immediate(variable_address[label], ADDRESS_SIZE);
				break;
			}
		}
	}
	pad_bytes(BINARY_SIZE + MEMORY_SIZE + STACK_SIZE);
    return 0;
}
