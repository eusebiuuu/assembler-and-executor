all: ./outputs/parser ./outputs/interpreter

./encoder/freq.txt: ./encoder/encodings_generator.py ./utils/instruction_name_counter.py
	cd './encoder'; \
	python './encodings_generator.py'

#./encoder/encodings.txt: ./encoder/freq.txt ./encoder/huffman.cpp
#	g++ -std=c++23 './encoder/huffman.cpp' -o './outputs/encoder'
#	cd './encoder';  \
#	'../outputs/encoder'

./utils/instruction_enum.hpp ./utils/dumb_parser.hpp: ./utils/utils_generator.py ./utils/instruction_name_counter.py
	cd './utils'; \
	python './utils_generator.py'

./outputs/parser: 	./utils/instruction_enum.hpp \
					./utils/dumb_parser.hpp \
					./assembler/parser.cpp \
					./utils/register_parser.hpp \
					./utils/registers_enum.hpp \
					./utils/instruction_size.hpp \
					./utils/cpu_spec.hpp \
					./encoder/encodings.txt

	g++ -std=c++23 './assembler/parser.cpp' -o './outputs/parser'

./outputs/interpreter: 	./utils/instruction_enum.hpp \
						./utils/dumb_parser.hpp \
						./executor/interpreter.cpp \
						./utils/register_parser.hpp \
						./utils/registers_enum.hpp \
						./utils/instruction_size.hpp \
						./utils/cpu_spec.hpp \
						./encoder/encodings.txt \
						./utils/int_registers.txt \
						./utils/float_registers.txt

	g++ -std=c++23 './executor/interpreter.cpp' -o './outputs/interpreter'


clean:
	-rm './encoder/freq.txt'
#	-rm './encoder/encodings.txt'

	-rm './outputs/encoder'

	-rm './outputs/parser'
	-rm './outputs/interpreter'

	-rm ./outputs/*.bin
	-rm ./outputs/*.out

	-rm './utils/instruction_enum.hpp'
	-rm './utils/dumb_parser.hpp'

test: ./run_tests.sh
	'./run_tests.sh'

remake: clean all

tests: remake test
