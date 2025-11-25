import os
DIR_OF_THIS_SCRIPT = os.path.abspath( os.path.dirname( __file__ ) )
fd = open(DIR_OF_THIS_SCRIPT + '/log.txt', 'a')
_print = print
def print(*args, file=None):
    _print(*args, file=fd)
    fd.flush()
print('start')

import json
# compile_commands = json.load(open('build/compile_commands.json', 'rb'))
# print('\n'.join([f"{c}" for c in compile_commands]))



def Settings(**kwargs):
    print(kwargs)
    return {
        'flags': [
            '-x', 'c', '-Wall', '-Wextra', '-Werror',
            '-DUNICODE', '-D_UNICODE',
            '-IC:/msys64/clang64/include/',
            # '-IC:/msys64/mingw64/include',
            # '-IC:/msys64/usr/include',
            ],
    }
