import sys
sys.path.insert(1, '../utils')

from instruction_name_counter import instruction_counter

total_instruction_count = sum(x[1] for x in instruction_counter)


print(f'{len(instruction_counter)} unique instructions from {total_instruction_count} samples')

print('\ninstruction\tcount\tprobability')
for (instruction, count) in instruction_counter:
    print(f'{instruction}\t', count, '%.4f'%round(count / total_instruction_count, 4), sep='\t')


with open('../encoder/freq.txt', 'w') as file:
    for (instruction, count) in instruction_counter:
        file.write(f'{instruction} {count / total_instruction_count}\n')