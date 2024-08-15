import re

LABEL_REGEX = '^\s*(\w*[\w:]{0,1})\s*'
COMMENT_REGEX = '[#.*$|$]'
NUMBER_ADDRESS = '\s*([-]{0,1}\d+)[(]{1}(\w+)[)]{1}\s*'
regDict = {}
regDict['R'] = rf'{LABEL_REGEX}(\w+)\s+(\w+),\s*(\w+),\s*(\w+)\s*' + COMMENT_REGEX
regDict['I'] = LABEL_REGEX + r'(\w+)\s+(\w+),\s*(\w+),\s*([-]{0,1}\d+)\s*|(\w+)\s+(\w+),' + NUMBER_ADDRESS + COMMENT_REGEX
regDict['S'] = ''


def getFormatInfo(instruction, format):
  matches = re.findall(regDict[format], instruction)
  if len(matches) > 1:
    raise SyntaxError('Invalid RISC-V instruction on line ???')
  return matches


def parseInstruction(codeLine):
  if re.fullmatch('^\s*\w*[\w:]{0,1}\s*$', codeLine):
    return True
  return False
  # check type of instruction (R, S, I, U, J, B)

completeRInstr = 'label: add r1, r2, r3 # hello'
completeIInstr = 'label: add rd, rs1, -3002 # eee'
completeIInstr = 'label: add rd, -20(rs1) # eee'
dummyInstr = 'label:  '

print(getFormatInfo(completeRInstr, 'R'))
print(getFormatInfo(completeIInstr, 'I'))
print(parseInstruction(dummyInstr))