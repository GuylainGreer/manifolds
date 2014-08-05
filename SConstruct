

env = Environment(CXX = 'clang++')
cpp_flags = '-std=c++1y -Wall -Wpedantic ' + \
    '-msse4 -march=native -mtune=native'
env.Program('test', Glob('*.cpp'),
            CPPFLAGS=cpp_flags)
