#ifndef MANIFOLDS_FUNCTIONS_EQUALS2_HH
#define MANIFOLDS_FUNCTIONS_EQUALS2_HH

namespace manifolds
{
  template <class T, class U>
  bool operator==(const T & t, const U & u)
  {
    return DispatchEquals(t, u);
  }
}
#endif
