import os
import re

aggregation = []

for folder in os.listdir('inputs'):
  for fileName in os.listdir(f'inputs/{folder}'):
    if fileName.startswith('asm'):
      with open(f'inputs/{folder}/{fileName}') as file:
        aggregation += file.readlines()

# print(aggregation)

opCounter = {}

for line in aggregation:
  if re.findall('^\s*$', line):
    continue
  if re.findall('^\s*\w+:', line):
    continue
  if re.findall('^\s*\.', line):
    continue
  if re.findall('^\s*#.*$', line):
    continue
  op = re.findall('^\s*(\S*)', line)
  opCounter[op[0]] = opCounter.get(op[0], 0) + 1

sum = 0
for elem in opCounter.values():
  sum += elem

for elem in opCounter.items():
  print(elem[0], elem[1] / sum)