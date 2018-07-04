import sys
import code

shell = code.InteractiveConsole(globals())

class OutputCatcher:
    def __init__(self):
        self.value = ''
    def write(self, txt):
        self.value += txt

outputCatcher = OutputCatcher()
sys.stdout = outputCatcher
sys.stderr = outputCatcher

shell.push('1000')

print(outputCatcher.value)

