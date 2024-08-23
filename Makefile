all: ./encoder/freq.txt ./encoder/encodings.txt ./utils/instruction_enum.hpp ./utils/dumb_parser.hpp ./outputs/parser ./outputs/interpreter


./encoder/freq.txt:
	cd './encoder'; python './encodings_generator.py'

./encoder/encodings.txt: ./encoder/freq.txt
	g++ -std=c++23 './encoder/huffman.cpp' -o './outputs/encoder'
	cd './encoder';  '../outputs/encoder'

./utils/instruction_enum.hpp ./utils/dumb_parser.hpp:
	cd './utils'; python './utils_generator.py'

./outputs/parser:
	g++ -std=c++23 './assembler/parser.cpp' -o './outputs/parser'

./outputs/interpreter:
	g++ -std=c++23 './executor/interpreter.cpp' -o './outputs/interpreter'


clean:
	-rm './encoder/freq.txt'
	-rm './encoder/encodings.txt'

	-rm './outputs/encoder'

	-rm './outputs/parser'
	-rm './outputs/interpreter'

	-rm ./outputs/*.bin
	-rm ./outputs/*.out

	-rm './utils/instruction_enum.hpp'
	-rm './utils/dumb_parser.hpp'

test:
	'./run_tests.sh'

remake: clean all

tests: remake test
