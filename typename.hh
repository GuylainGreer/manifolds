#ifndef GNU_TYPENAME_HH
#define GNU_TYPENAME_HH

#include <cxxabi.h>
#include <string>
#include <typeinfo>

template <class T>
std::string get_cpp_name()
{
    char * name = abi::__cxa_demangle(typeid(T).name(),0,0,0);
    std::string real_name = name;
    free(name);
    return real_name;
}

#endif
