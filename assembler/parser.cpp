#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include "instruction_enum.h"
#include "dumb_parser.hpp"
using namespace std;
ifstream fin("../inputs/04_Reverse_a_string/asm.s");
// ifstream fin("../inputs/12_Calling_a_C_Function_from_Assembly/asm.s");

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

int main() {
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
				while (pos < (int) line.size() && isalnum(line[pos])) {
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

			switch (instruction_to_enum(instruction)) {
				case InstructionType::add: {
					array<string, 3> registers;
					for (int _ = 0; _ < 3; ++_) {
						jumpOverNonAlNum();
						registers[_] = captureWord();
					}
					for (auto elem : registers) {
						cout << elem << ' ';
					}
					cout << '\n';
					break;
				}
				case InstructionType::li: {
					string currRegister;
					jumpOverNonAlNum();
					currRegister = captureWord();
					jumpOverNonAlNum();
					int num = captureNumber();
					cout << currRegister << ' ' << num << '\n';
					break;
				}
				case InstructionType::addi: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					string reg2 = captureWord();
					jumpOverNonAlNum();
					int num = captureNumber();
					cout << reg1 << ' ' << reg2 << ' ' << num << '\n';
					break;
				}
				case InstructionType::mv: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					string reg2 = captureWord();
					cout << reg1 << ' ' << reg2 << '\n';
					break;
				}
				case InstructionType::ret: {
					break;
				}
				case InstructionType::beqz: {
					jumpOverNonAlNum();
					string reg = captureWord();
					jumpOverNonAlNum();
					string label = captureWord();
					int direction = label.back() == 'f' ? 1 : -1;
					label = label.substr(0, label.size() - 1);
					cout << reg << ' ' << label << ' ' << direction << '\n';
					break;
				}
				case InstructionType::lb: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					int offset = captureNumber();
					pos++;
					string reg2 = captureWord();
					cout << reg1 << ' ' << reg2 << ' ' << offset << '\n';
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
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					int offset = captureNumber();
					pos++;
					string reg2 = captureWord();
					cout << reg1 << ' ' << reg2 << ' ' << offset << '\n';
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
					array<string, 3> registers;
					for (int _ = 0; _ < 3; ++_) {
						jumpOverNonAlNum();
						registers[_] = captureWord();
					}
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
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					int offset = captureNumber();
					pos++;
					string reg2 = captureWord();
					cout << reg1 << ' ' << reg2 << ' ' << offset << '\n';
					break;
				}
				case InstructionType::call: {
					jumpOverNonAlNum();
					string label = captureLabel();
					cout << label << '\n';
				}
				case InstructionType::ld: {
					jumpOverNonAlNum();
					string reg1 = captureWord();
					jumpOverNonAlNum();
					int offset = captureNumber();
					pos++;
					string reg2 = captureWord();
					cout << reg1 << ' ' << reg2 << ' ' << offset << '\n';
					break;
				}
			}
		}
	}
    return 0;
}
