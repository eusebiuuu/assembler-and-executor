#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include <unordered_map>
#include <bitset>

#include "../utils/instruction_enum.hpp"
#include "../utils/dumb_parser.hpp"
#include "../utils/register_parser.hpp"
#include "../utils/registers_enum.hpp"
#include "../utils/instruction_size.hpp"
#include "../utils/cpu_spec.hpp"

std::ofstream fout;
std::ifstream enc;

std::unordered_map<InstructionType, std::string> encodings;
std::vector<std::pair<std::string, int>> label_address;
std::vector<std::pair<std::string, std::string>> variables_list;
std::vector<std::pair<std::string, int>> store_spaces;
std::unordered_map<std::string, int> variable_address;

int current_bit_offset = 0, current_address = 0;

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
	emit_immediate((int)reg, INT_REGISTER_BITS);
}

void emit_register(RegisterFloatType reg) {
	emit_immediate((int)reg, FLOAT_REGISTER_BITS);
}

void emit_variables() {
	for (auto elem : variables_list) {
		variable_address[elem.first] = current_address;

		int sz = elem.second.size();
		std::string real_string;
		for (int i = 0; i < sz; ++i) {
			if (elem.second[i] == '\\' && i + 1 < sz && elem.second[i + 1] == 'n') {
				real_string += '\n';
				i++;
				continue;
			}
			real_string += elem.second[i];
		}
		
		real_string += '\0';
		
		for (char c : real_string) {
			emit_immediate(c, 8);
		}
	}

	for (auto elem : store_spaces) {
		variable_address[elem.first] = current_address;
		pad_bytes(current_address + elem.second);
	}
}



void jump_over_spaces(int &pos, std::string line) {
	while (pos < (int) line.size() && isspace(line[pos])) {
		pos++;
	}
}

std::string get_word(int &pos, std::string line) {
	std::string word;
	while (pos < (int) line.size() && (isalnum(line[pos]) || line[pos] == '_' || line[pos] == '.')) {
		word += line[pos++];
	}
	return word;
}

int get_number(int &pos, std::string line) {
	int num = 0;
	while (pos < (int) line.size() && '0' <= line[pos] && line[pos] <= '9') {
		num = num * 10 + (line[pos++] - '0');
	}
	return num;
}

bool is_directive(std::string line) {
	int pos = 0;
	jump_over_spaces(pos, line);
	if (pos >= (int) line.size() || line[pos] != '.') {
		return false;
	}
	return true;
}

std::vector<std::string> get_directive(std::string line) {
	std::vector<std::string> info;
	int pos = line.find('.') + 1;
	info.push_back(get_word(pos, line));
	jump_over_spaces(pos, line);

	pos += info[0] == "section";

	info.push_back(get_word(pos, line));
	return info;
}

bool is_label(std::string line) {
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

std::string get_label(std::string line) {
	int pos = 0;
	jump_over_spaces(pos, line);
	std::string aux = get_word(pos, line);
	return aux;
}

void parse_rodata(std::ifstream &fin) {
	std::string line;
	while (getline(fin, line)) {
		if (!is_label(line)) break;

		std::string label = get_label(line);
		int pos = line.find(":") + 1;
		jump_over_spaces(pos, line);
		std::string var_type = get_word(pos, line);

		if (var_type == ".asciz") {
			jump_over_spaces(pos, line);
			pos++;
			int ending_pos = line.find("\"", pos);
			std::string str = line.substr(pos, ending_pos - pos);
			variables_list.push_back({label, str});
		} else if (var_type == ".space") {
			jump_over_spaces(pos, line);
			int memory_size = get_number(pos, line);
			store_spaces.push_back({label, memory_size});
		} else {
			std::cerr << "[ERROR]: Invalid variable type\n";
		}
	}
}

void find_labels_addresses(std::ifstream &fin) {
	std::string line;
    while (getline(fin, line)) {
		if (is_directive(line)) {
			auto info = get_directive(line);
			if (info[0] == "section" && info[1] == "rodata") {
				parse_rodata(fin);
			}
		} else if (is_label(line)) {
			std::string label = get_label(line);
			label_address.push_back({label, current_address});
		} else {
			int pos = 0;
			jump_over_spaces(pos, line);
			if (pos >= (int) line.size() || line[pos] == '#') {
				continue;
			}

			std::string instruction = get_word(pos, line);
            current_address += get_instruction_size(instruction_to_enum(instruction));
        }
    }
    fin.clear();
    fin.seekg(0, fin.beg);
    current_address = 0;
}

int get_closest_address(std::string label, int address, int dir) {
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

int get_label_address(std::string label) {
	for (auto elem : label_address) {
		if (elem.first == label) {
			return elem.second;
		}
	}
	return -1;
}

void find_instructions() {
	std::string instr, currEnc;
	while (enc >> instr >> currEnc) {
		encodings[instruction_to_enum(instr)] = currEnc;
	}
}

int main(int argc, char *argv[]) {
	std::ifstream fin(argv[1]);
	fout.open(argv[2]);
	enc.open(argv[3]);

	std::cout << "Opening files and store the encodings...\n";

	find_instructions();

	std::cout << "Finished!\n";

	std::cout << "Start parsing...\n";

	current_address = MEMORY_SIZE;
	find_labels_addresses(fin);
	
	int main_address = get_label_address("main");
	emit_instruction(InstructionType::j);
	emit_immediate(main_address, ADDRESS_SIZE);
	pad_current_byte();

	emit_variables();

	pad_bytes(MEMORY_SIZE);

    std::string line;
	while (std::getline(fin, line)) {
		if (is_directive(line) || is_label(line)) {
			continue;
		}
		int pos = 0;
		jump_over_spaces(pos, line);
		if (pos >= (int) line.size() || line[pos] == '#') {
			continue;
		}

		std::string instruction = get_word(pos, line);
		std::cout << "Instruction: " << instruction << '\n';

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

		auto captureWord = [&pos, line]() -> std::string {
			std::string word;
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

		auto captureLabel = [&pos, line]() -> std::string {
			std::string label;
			while (pos < (int) line.size() && (isalnum(line[pos]) || line[pos] == '_')) {
				label += line[pos++];
			}
			return label;
		};

		auto getRegisters = [captureWord, jumpOverNonAlNum](const int cnt) -> std::vector<std::string> {
			std::vector<std::string> registers;
			for (int _ = 0; _ < cnt; ++_) {
				jumpOverNonAlNum();
				registers.push_back(captureWord());
			}
			return registers;
		};

		auto get2RegistersOffset = [captureNumber, captureWord, &pos, jumpOverNonAlNum]() -> std::pair<std::array<std::string, 2>, int> {
			std::array<std::string, 2> regs;
			jumpOverNonAlNum();
			regs[0] = captureWord();
			jumpOverNonAlNum();
			int offset = captureNumber();
			pos++;
			regs[1] = captureWord();
			return {regs, offset};
		};

		InstructionType curr_type = instruction_to_enum(instruction);
		emit_instruction(curr_type);

		switch (curr_type) {
			case InstructionType::add: {
				auto registers = getRegisters(3);
				
				for (auto elem : registers) {
					emit_register(parseIntRegister(elem));
				}
				break;
			}
			case InstructionType::li: {
				std::string currRegister;
				jumpOverNonAlNum();
				currRegister = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				
				emit_register(parseIntRegister(currRegister));
				emit_immediate(num, 8);
				break;
			}
			case InstructionType::addi: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 13);
				break;
			}
			case InstructionType::mv: {
				auto registers = getRegisters(2);

				
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseIntRegister(registers[1]));
				break;
			}
			case InstructionType::ret: {
				
				break;
			}
			case InstructionType::beqz: {
				jumpOverNonAlNum();
				std::string reg = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				
				emit_register(parseIntRegister(reg));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::lb: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;

				
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::j: {
				jumpOverNonAlNum();
				std::string label = captureLabel();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::sb: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;

				
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::bge: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);

				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::sub: {
				auto registers = getRegisters(3);

				
				for (auto elem : registers) {
					emit_register(parseIntRegister(elem));
				}
				break;
			}
			case InstructionType::srai: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();

				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 2 + 8);
				break;
			}
			case InstructionType::sd: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::call: {
				jumpOverNonAlNum();
				std::string label = captureLabel();
				
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
				
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::slli: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				int num = captureNumber();
				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(num, 2);
				break;
			}
			case InstructionType::lw: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				
				emit_register(parseIntRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3 + 8);
				break;
			}
			case InstructionType::bnez: {
				jumpOverNonAlNum();
				std::string reg = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				
				emit_register(parseIntRegister(reg));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::ble: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::fld: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				
				emit_register(parseFloatRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 4);
				break;
			}
			case InstructionType::fsub_d: {
				auto registers = getRegisters(3);
				
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fadd_d: {
				auto registers = getRegisters(3);
				
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fmul_d: {
				auto registers = getRegisters(3);
				
				for (auto elem : registers) {
					emit_register(parseFloatRegister(elem));
				}
				break;
			}
			case InstructionType::fsqrt_d: {
				auto registers = getRegisters(2);
				
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				break;
			}
			case InstructionType::fsw: {
				auto info = get2RegistersOffset();
				auto regs = info.first;
				int offset = info.second;
				
				emit_register(parseFloatRegister(regs[0]));
				emit_register(parseIntRegister(regs[1]));
				emit_immediate(offset, 3);
				break;
			}
			case InstructionType::fmv_s: {
				auto registers = getRegisters(2);
				
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				break;
			}
			case InstructionType::flt_s: {
				auto registers = getRegisters(3);
				
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				emit_register(parseFloatRegister(registers[2]));
				break;
			}
			case InstructionType::fgt_s: {
				auto registers = getRegisters(3);
				
				emit_register(parseIntRegister(registers[0]));
				emit_register(parseFloatRegister(registers[1]));
				emit_register(parseFloatRegister(registers[2]));
				break;
			}
			case InstructionType::bgt: {
				jumpOverNonAlNum();
				std::string reg1 = captureWord();
				jumpOverNonAlNum();
				std::string reg2 = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				int direction = label.back() == 'f' ? 1 : -1;
				label = label.substr(0, label.size() - 1);
				
				emit_register(parseIntRegister(reg1));
				emit_register(parseIntRegister(reg2));
				emit_immediate(get_closest_address(label, current_address, direction), ADDRESS_SIZE);
				break;
			}
			case InstructionType::fmv_s_x: {
				auto registers = getRegisters(2);
				
				emit_register(parseFloatRegister(registers[0]));
				emit_register(parseIntRegister(registers[1]));
				break;
			}
			//case InstructionType::fmul_s: {
			//	auto registers = getRegisters(3);
			//	
			//	for (auto elem : registers) {
			//		emit_register(parseFloatRegister(elem));
			//	}
			//	break;
			//}
			//case InstructionType::fadd_s: {
			//	auto registers = getRegisters(3);
			//	
			//	for (auto elem : registers) {
			//		emit_register(parseFloatRegister(elem));
			//	}
			//	break;
			//}
			//case InstructionType::flw: {
			//	auto info = get2RegistersOffset();
			//	auto regs = info.first;
			//	int offset = info.second;
			//	
			//	emit_register(parseFloatRegister(regs[0]));
			//	emit_register(parseIntRegister(regs[1]));
			//	emit_immediate(offset, 3);
			//	break;
			//}
			case InstructionType::la: {
				jumpOverNonAlNum();
				std::string reg = captureWord();
				jumpOverNonAlNum();
				std::string label = captureWord();
				
				emit_register(parseIntRegister(reg));
				emit_immediate(variable_address[label], ADDRESS_SIZE);
				break;
			}
		}
	}
	pad_bytes(BINARY_SIZE + MEMORY_SIZE + STACK_SIZE);
	std::cout << "Parsing finished!\n";
    return 0;
}
