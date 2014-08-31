compiler = 'clang++'
error_limit = '-fmax-errors=1' \
              if compiler == 'g++' else \
                 '-ferror-limit=1'
env = Environment(CXX = compiler)
cpp_flags = ' '.join(['-std=c++1y',
                     '-Wall',
                      '-Wpedantic',
                      '-ftemplate-depth=1024',
                      '-ftemplate-backtrace-limit=0',
                      '-msse4 -march=native',
                      '-mtune=native',
                      error_limit,
                      #'-DPRINT_SIMPLIFIES',
                  ])
env.Program('test', Glob('*.cpp'),CPPFLAGS=cpp_flags)
