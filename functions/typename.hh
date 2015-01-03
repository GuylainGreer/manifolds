#ifndef GNU_TYPENAME_HH
#define GNU_TYPENAME_HH

#include <cxxabi.h>
#include <string>
#include <typeinfo>

template <class T> std::string get_cpp_name() {
  char *name = abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
  std::string real_name = name;
  free(name);
  auto i = real_name.find("manifolds::");
  while (i != std::string::npos) {
    real_name.erase(real_name.begin() + i, real_name.begin() + i + 11);
    i = real_name.find("manifolds::");
  }
  return real_name;
}

#endif
