import os

compiler = 'g++-4.9'
error_limit = '-fmax-errors=1' \
              if 'g++' in compiler else \
                 '-ferror-limit=1'
env = Environment(CXX = compiler)

cpp_flags = ' '.join(['-std=c++1y',
                     '-Wall',
                      '-Wpedantic',
                      '-ftemplate-depth=512',
                      '-ftemplate-backtrace-limit=0',
                      '-msse4 -march=native',
                      '-mtune=native',
                      error_limit,
                      #'-DPRINT_SIMPLIFIES',
                      '-g',
		      '-fvisibility=hidden',
                  ])

sources = []
for root, dirs, files in os.walk('.'):
    sources.extend([os.path.join(root, f) for f in files if \
                    os.path.splitext(f)[1] == '.cpp'])

env.Program('test_exec', sources, CPPFLAGS=cpp_flags,
            CPPPATH = ['.'],
	    LIBS = ['boost_filesystem', 'boost_system'])
