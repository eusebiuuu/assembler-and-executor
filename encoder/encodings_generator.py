import os

instruction_counter = {}

for folder in os.listdir('../inputs'):
    with open(f'../inputs/{folder}/asm.s') as file:
        for line in file.readlines():
            if not line.isspace() and line[0].isspace():
                word = line.split(maxsplit=1)[0]
                if word[0] != '#':
                    instruction_counter[word] = instruction_counter.get(word, 0) + 1

total_instruction_count = sum(instruction_counter.values())

for (instruction, count) in sorted(instruction_counter.items(), key = lambda x: -x[1]):
    print(f'{instruction}\t', count, '%.4f'%round(count / total_instruction_count, 4), sep='\t')



with open('../encoder/freq_temp.txt', 'w') as file:
    for (instruction, count) in sorted(instruction_counter.items(), key = lambda x: -x[1]):
        file.write(f'{instruction} {count / total_instruction_count}\n')