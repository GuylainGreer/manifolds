

env = Environment(CXX = 'clang++')
cpp_flags = '-std=c++1y -Wall -Wpedantic -ftemplate-depth=10000 -ftemplate-backtrace-limit=0 ' + \
    '-msse4 -march=native -mtune=native'# -DPRINT_SIMPLIFIES '
env.Program('test', Glob('*.cpp'),CPPFLAGS=cpp_flags)
