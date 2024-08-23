import os
from collections import defaultdict

instruction_counter = defaultdict(int)

for folder in os.listdir('../inputs'):
    with open(f'../inputs/{folder}/asm.s') as file:
        for line in file.readlines():
            if not line.isspace() and line[0].isspace():
                word = line.split(maxsplit=1)[0]
                if word[0] != '#':
                    instruction_counter[word] += 1


instruction_counter = sorted(instruction_counter.items(), key = lambda x: -x[1])