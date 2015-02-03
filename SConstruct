import os

compiler = 'clang++'
error_limit = '-ferror-limit=1' \
              if 'clang++' in compiler else \
                 '-fmax-errors=1'
env = Environment(CXX = compiler)

cpp_flags = ' '.join(['-std=c++1y',
                      '-Wall',
                      '-Wpedantic',
                      '-Werror',
                      '-ftemplate-depth=512',
                      '-ftemplate-backtrace-limit=0',
                      '-msse4 -march=native',
                      '-mtune=native',
                      error_limit,
                      #'-DPRINT_SIMPLIFIES',
                      '-fvisibility=hidden',
                      ])
if 'clang' not in compiler:
    cpp_flags = cpp_flags + ' -g'
else:
    cpp_flags = cpp_flags + ' -O3'

sources = []
for root, dirs, files in os.walk('.'):
    sources.extend([os.path.join(root, f) for f in files if \
                    os.path.splitext(f)[1] == '.cpp'])

env.Program('test_exec', sources, CPPFLAGS=cpp_flags,
            CPPPATH = ['.'],
	    LIBS = ['boost_filesystem', 'boost_system'])
