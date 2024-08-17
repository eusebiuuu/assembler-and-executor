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

using namespace std;
ofstream fout("out");

// ifstream fin("../inputs/11_Vector_x_Matrix/asm.s");
ifstream fin("../inputs/04_Reverse_a_string/asm.s");
ifstream enc("../encoder/encodings.txt");
unordered_map<InstructionType, string> encodings;

int current_bit_offset = 0;

void join_bit(bool bit) {
	static std::bitset<8> current_byte;
	current_byte[7 - current_bit_offset] = bit;
	current_bit_offset++;
	if(current_bit_offset >= 8) {
		fout << (unsigned char)current_byte.to_ulong();
		current_bit_offset = 0;
	}
}

void pad_current_byte() {
	while(current_bit_offset) {
		join_bit(0);
	}
}

void emit_instruction(InstructionType instruction) {
	pad_current_byte();
	for(char c : encodings[instruction]) {
		join_bit(c - '0');
	}
}

void emit_register(RegisterIntType reg) {
	for (int bit = 3; bit >= 0; --bit) {
		bool val = reg & (1 << bit);
		join_bit(val);
	}
}

void emit_register(RegisterFloatType reg) {
	for (int bit = 1; bit >= 0; --bit) {
		bool val = reg & (1 << bit);
		join_bit(val);
	}
}

void emit_immediate(int num, const int bits_count) {
	for (int i = bits_count - 1; i >= 0; --i) {
		bool bit = num & (1 << i);
		join_bit(bit);
	}
}

void parseRoData() {
	string line;
	while (getline(fin, line)) {
		if (line.empty() || line[0] == '.') break;
		string label, str;
		auto pos = line.find(":");
		label = line.substr(0, pos);
		pos += 10;
		str = line.substr(pos, line.size() - 1 - pos);
		cout << line << '\n' << label << '\n' << str << '\n';
	}
}

void getEncodings() {
	string instr, currEnc;
	while (enc >> instr >> currEnc) {
		encodings[instruction_to_enum(instr)] = currEnc;
		cout << instr << ' ' << currEnc << '\n';
	}
}

int main() {
	getEncodings();
    string line;
	while (getline(fin, line)) {
		if (line[0] == '.') {
			if (line[9] != '.' || line[10] != 'r') {
				continue;
			}

			parseRoData();
		} else if (isalnum(line[0])) {
			auto pos = line.find(":");
			string label = line.substr(0, pos);
			cout << "Label: " << label << '\n';
		} else {
			int pos = -1;
			for (int i = 0; i < (int) line.size(); ++i) {
				if (isspace(line[i])) continue;
				pos = i;
				break;
			}

			if (pos == -1 || line[pos] == '#') {
				continue;
			}

			string instruction;
			while (pos < (int) line.size() && !isspace(line[pos])) {
				instruction += line[pos++];
			}
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

			auto get2RegistersOffset = [captureNumber, captureWord, &pos, jumpOverNonAlNum]() 
			-> pair<array<string, 2>, int> {
				array<string, 2> regs;
				jumpOverNonAlNum();
				regs[0] = captureWord();
				jumpOverNonAlNum();
				int offset = captureNumber();
				pos++;
				regs[1] = captureWord();
				return {regs, offset};
			};

			switch (instruction_to_enum(instruction)) {
				case InstructionType::add: {
					auto registers = getRegisters(3);
					// cout << registers[0] << ' ' << registers[1] << ' ' << registers[2] << '\n';
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
					emit_immediate(num, 5);
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
					break;
				}
				case InstructionType::lb: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					emit_instruction(InstructionType::lb);
					emit_register(parseIntRegister(reg[0]));
					emit_register(parseIntRegister(reg[1]));
					emit_immediate(offset, 7);
					break;
				}
				case InstructionType::j: {
					jumpOverNonAlNum();
					string label = captureLabel();
					int direction = label.back() == 'f' ? 1 : -1;
					label = label.substr(0, label.size() - 1);
					cout << label << ' ' << direction << '\n';
					break;
				}
				case InstructionType::sb: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
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
					cout << reg1 << ' ' << reg2 << ' ' << label << ' ' << direction << '\n';
					break;
				}
				case InstructionType::sub: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::srai: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					string reg2 = captureWord();
					jumpOverNonAlNum();
					int num = captureNumber();
					cout << reg1 << ' ' << reg2 << ' ' << num << '\n';
					break;
				}
				case InstructionType::sd: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::call: {
					jumpOverNonAlNum();
					string label = captureLabel();
					cout << label << '\n';
					break;
				}
				case InstructionType::ld: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::slli: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					string reg2 = captureWord();
					jumpOverNonAlNum();
					int num = captureNumber();
					cout << reg1 << ' ' << reg2 << ' ' << num << '\n';
					break;
				}
				case InstructionType::lw: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::bnez: {
					jumpOverNonAlNum();
					string reg = captureWord();
					jumpOverNonAlNum();
					string label = captureWord();
					int direction = label.back() == 'f' ? 1 : -1;
					label = label.substr(0, label.size() - 1);
					cout << reg << ' ' << label << ' ' << direction << '\n';
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
					cout << reg1 << ' ' << reg2 << ' ' << label << ' ' << direction << '\n';
					break;
				}
				case InstructionType::fld: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::fsub_d: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::fadd_d: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::fmul_d: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::fsqrt_d: {
					auto registers = getRegisters(2);
					cout << registers[0] << ' ' << registers[1] << '\n';
					break;
				}
				case InstructionType::fsw: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::fmv_s: {
					auto registers = getRegisters(2);
					cout << registers[0] << ' ' << registers[1] << '\n';
					break;
				}
				case InstructionType::flt_s: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::fgt_s: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
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
					cout << reg1 << ' ' << reg2 << ' ' << label << ' ' << direction << '\n';
					break;
				}
				case InstructionType::fmv_s_x: {
					auto registers = getRegisters(2);
					cout << registers[0] << ' ' << registers[1] << '\n';
					break;
				}
				case InstructionType::fmul_s: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::fadd_s: {
					auto registers = getRegisters(3);
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::flw: {
					auto info = get2RegistersOffset();
					auto regs = info.first;
					int offset = info.second;
					cout << regs[0] << ' ' << regs[1] << ' ' << offset << '\n';
					break;
				}
				case InstructionType::la: {
					jumpOverNonAlNum();
					string reg = captureWord();
					jumpOverNonAlNum();
					string label = captureWord();
					cout << reg << ' ' << label << '\n';
					break;
				}
			}
		}
	}
	pad_current_byte();
    return 0;
}
