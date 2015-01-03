#ifndef MANIFOLDS_FUNCTIONS_TEMPLATE_METAPROGRAMMING_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TEMPLATE_METAPROGRAMMING_UTILITIES_HH

#include <complex>
#include "typename.hh"
#include <iostream>

namespace manifolds {
template <template <class...> class Variadic, class...>
struct IsVariadic : std::false_type {};

template <template <class...> class V, class... Fs, class... Fs2>
struct IsVariadic<V, V<Fs...>, Fs2...> : std::true_type {};

template <class T> struct Identity {
  typedef T type;
};

template <int N> using int_ = std::integral_constant<int, N>;

template <bool B> using bool_ = std::integral_constant<bool, B>;

template <class> struct is_complex : std::false_type {};

template <class Type>
struct is_complex<std::complex<Type> > : std::true_type {};

template <class... Args> struct and_;

template <class Arg, class... Args>
struct and_<Arg, Args...> : std::conditional<Arg::value, and_<Args...>,
                                             std::false_type>::type {};

template <class Arg> struct and_<Arg> : bool_<Arg::value> {};

template <> struct and_<> : std::true_type {};

template <class... Args> struct or_;

template <class Arg, class... Args>
struct or_<Arg, Args...> : std::conditional<Arg::value, std::true_type,
                                            or_<Args...> >::type {};

template <class Arg> struct or_<Arg> : bool_<Arg::value> {};

template <> struct or_<> : std::false_type {};

template <class...> struct holder {};

template <class, class, class = void> struct are_same;

template <class... Arg1s, class... Arg2s>
struct are_same<
    holder<Arg1s...>, holder<Arg2s...>,
    typename std::enable_if<sizeof...(Arg1s) !=
                            sizeof...(Arg2s)>::type> : std::false_type {};

template <class... Arg1s, class... Arg2s>
struct are_same<
    holder<Arg1s...>, holder<Arg2s...>,
    typename std::enable_if<sizeof...(Arg1s) == sizeof...(Arg2s)>::
        type> : and_<typename std::is_same<Arg1s, Arg2s>::type...> {};

template <int...> struct max;

template <int v, int... values> struct max<v, values...> {
  static const int sub_max = max<values...>::value;
  static const int value = v > sub_max ? v : sub_max;
  typedef int_<value> type;
};

template <int v> struct max<v> : int_<v> {};

template <> struct max<> : int_<0> {};

template <int...> struct sum;

template <int i, int... is>
struct sum<i, is...> : int_<i + sum<is...>::value> {};

template <> struct sum<> : int_<0> {};

template <int, int...> struct sum_n;

template <int n, int i, int... is> struct sum_n<n, i, is...> {
  static const int next = n ? n - 1 : 0;
  static const int adding = n ? i : 0;
  static const int value = adding + sum_n<next, is...>::value;
  typedef int_<value> type;
};

template <> struct sum_n<0> : int_<0> {};

template <int n, class... Ts> struct nth;

template <int n, class T, class... Ts>
struct nth<n, T, Ts...> : nth<n - 1, Ts...> {};

template <class T, class... Ts> struct nth<0, T, Ts...> {
  typedef T type;
};

template <class T, class... Ts> struct first {
  typedef T type;
};

template <class... Ts> struct last {
  typedef typename nth<sizeof...(Ts) - 1, Ts...>::type type;
};

template <class> struct seq_size;

template <class T, T... ts>
struct seq_size<std::integer_sequence<T, ts...> > : int_<sizeof...(ts)> {};

namespace detail {
template <class T> struct sfinae_enable : std::true_type {};
}

#define FUNC_CHECK_DEF(mname, code)                                            \
  namespace detail {                                                           \
  template <class T> sfinae_enable<code> mname##_detail(int);                  \
  template <class> std::false_type mname##_detail(long);                       \
  }                                                                            \
  template <class T> struct mname : decltype(detail::mname##_detail<T>(0)) {};

FUNC_CHECK_DEF(has_coeff_function,
               decltype(std::declval<T>().Coeff(std::declval<int>(),
                                                std::declval<int>())))

template <class T> struct is_stateless : bool_<T::stateless> {};

template <class T>
struct has_abelian_arithmetic : bool_<T::abelian_arithmetic> {};

template <class... Types> struct common_type {
  template <class T, bool b = std::is_unsigned<T>::value &&std::is_integral<
                         T>::value &&or_<std::is_signed<Types>...>::value>
  struct make_signed {
    typedef typename std::make_signed<T>::type type;
  };

  template <class T> struct make_signed<T, false> {
    typedef T type;
  };

  typedef typename std::common_type<Types...>::type std_type;
  typedef typename std::conditional<(std::is_unsigned<std_type>::value &&
                                         or_<std::is_signed<Types>...>::value),
                                    typename make_signed<std_type>::type,
                                    std_type>::type type;
};
}

#endif
