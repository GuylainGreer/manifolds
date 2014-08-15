#ifndef MANIFOLDS_FUNCTIONS_TEMPLATE_METAPROGRAMMING_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TEMPLATE_METAPROGRAMMING_UTILITIES_HH

#include <tuple>

namespace manifolds
{
  template <class T>
  struct is_stateless
  {
    static const bool value = T::stateless;
    typedef std::integral_constant<
      bool, value> type;
  };

  template <class T>
  struct has_abelian_arithmetic
  {
    static const bool value = T::abelian_arithmetic;
    typedef std::integral_constant<
      bool, value> type;
  };

  template <class...Args>
  struct and_;

  template <class Arg, class ... Args>
  struct and_<Arg,Args...> : std::conditional<
    Arg::value, and_<Args...>, std::false_type>::type{};

  template <class Arg>
  struct and_<Arg> : std::conditional<
    Arg::value, std::true_type, std::false_type>::type{};

  template <>
  struct and_<> : std::true_type{};

  template <int N>
  using int_ = std::integral_constant<int, N>;

  template <bool B>
  using bool_ = std::integral_constant<bool, B>;

  template <int v, int ... values>
  struct max
  {
    static const int sub_max =
      max<values...>::value;
    static const int value =
      v > sub_max ? v : sub_max;
    typedef int_<value> type;
  };

  template <int v>
  struct max<v>
  {
    static const int value = v;
    typedef int_<value> type;
  };

  template <int n, class ... Ts>
  struct nth
  {
    typedef typename std::tuple_element<
      n, std::tuple<Ts...>>::type type;
  };

  template <class T, class ... Ts>
  struct first
  {
    typedef T type;
  };

  template <class ... Ts>
  struct last
  {
    typedef typename nth<
      sizeof...(Ts)-1, Ts...>::type type;
  };

  namespace detail
  {
    template <class T>
    struct sfinae_enable : std::true_type{};
  }

#define FUNC_CHECK_DEF(mname, code)		\
  namespace detail				\
  {						\
    template <class T>				\
      sfinae_enable<code>			\
      mname##_detail(int);			\
      template <class>				\
	std::false_type mname##_detail(long);	\
  }						\
  template <class T>				\
  struct mname :				\
    decltype(detail::mname##_detail<T>(0)){};	\

  FUNC_CHECK_DEF(has_coeff_function,
		 decltype(std::declval<T>().
			  Coeff(std::declval<int>(),
				std::declval<int>())))

  FUNC_CHECK_DEF(is_gettable,
		 decltype(std::get<0>(std::declval<T>())))
}

#endif
