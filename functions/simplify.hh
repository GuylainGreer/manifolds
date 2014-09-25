#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_HH

#include "streaming2.hh"
#include <boost/type_traits/is_same.hpp>
#include <iostream>
#include <unordered_map>
#include <typeinfo>
#include <boost/any.hpp>
#include <boost/filesystem/convenience.hpp>
#include <tuple>

namespace manifolds {

  static const int num_simplification_levels = 5;

#ifdef PRINT_SIMPLIFIES
  template <class Tuple>
  class OutputHolder
  {
    Tuple t;
  public:
    OutputHolder(const Tuple & t):t(t){}
    template <class C>
    auto operator<<(C c) ->
      OutputHolder<decltype(push_back(t, c))>
    {
      return {push_back(t, c)};
    }
    void Flush() const
    {
      std::cout << get<0>(t);
      OutputHolder<decltype(remove_element<0>(t))>
	(remove_element<0>(t)).Flush();
    }
  };
  template <>
  class OutputHolder<std::tuple<>>
  {
  public:
    OutputHolder(std::tuple<>){}
    template <class C>
    auto operator<<(C c)
    {
      return OutputHolder<std::tuple<C>>(std::make_tuple(c));
    }
    void Flush() const{}
  };
  
#  define SIMPLIFY_INFO(msg)			\
  (OutputHolder<std::tuple<>>(std::tuple<>()) <<	\
   boost::filesystem::basename(__FILE__)	\
   << ":" << __LINE__ << " " << msg).Flush()
#else
#  define SIMPLIFY_INFO(msg)
#endif

  template <class A,
	    int level,
	    class ForEnableIf = void>
  struct Simplification
  {
    typedef A type;
    static type Combine(A a)
    {
      SIMPLIFY_INFO("No simplification found\n");
      return a;
    }
  };

  template <class>
  struct SimplificationLevel;

  template <class A, int level, class B>
  struct SimplificationLevel<Simplification<A,level,B>>:
    int_<level>{};

  template <int iter>
  struct SimplificationWrapper
  {
    template <class F, int level>
    static auto Simplify(F f, F, int_<level>)
    {
      SIMPLIFY_INFO("Done level " << level << ", "
		    << "moving to level " << level+1 << '\n');
      return SimplificationWrapper<iter+1>::
	Simplify(f, int_<level+1>());
    }

    template <class F, class G, int level>
    static auto Simplify(F f, G, int_<level>)
    {
      return SimplificationWrapper<iter+1>::
	Simplify(f, int_<0>());
    }

    template <class F, int level>
    static auto Simplify(F f, int_<level>)
    {
      return Simplify(Simplification<F,level>::Combine(f), f,
		      int_<level>());
    }

    template <class F>
    static auto Simplify(F f, int_<num_simplification_levels>)
    {
      SIMPLIFY_INFO("Done: " << f << "\n\n");
      return f;
    }
  };

  template <class T>
  using SimplifiedType =
     decltype(SimplificationWrapper<0>::
	      Simplify(std::declval<T>(), int_<0>()));

  template <class F>
  auto Simplify(F f, std::false_type)
  {
    return SimplificationWrapper<0>::
      Simplify(f, int_<0>());
  }

  template <class F>
  auto Simplify(F f, std::true_type)
  {
    static auto hasher =
      [](const auto & a) {
      return std::hash<const char*>()(a.type().name());
    };
    static auto equaler =
      [](const auto & a, const auto & b) {
      return a.type() == b.type();
    };
    static std::unordered_map<
      boost::any, boost::any,
      decltype(hasher),
      decltype(equaler)> already_done(30, hasher, equaler);
    auto i = already_done.find(f);
    if(i == already_done.end())
      {
	auto r = Simplify(f, std::false_type());
	already_done[f] = r;
	return r;
      }
    typedef SimplifiedType<F> RType;
    SIMPLIFY_INFO("Already done, skipping to result "
		  << boost::any_cast<RType>(i->second)
		  << '\n');
    return boost::any_cast<RType>(i->second);
  }

  template <class F>
  auto Simplify(F f)
  {
    SIMPLIFY_INFO("Simplifying: " << f << "\n");
#ifdef PRINT_SIMPLIFIES
    return Simplify(f, bool_<F::stateless>());
#else
    return SimplificationWrapper<0>::Simplify(f, int_<0>());
#endif
  }

  template <class T>
  using Simplifies = bool_<
    !std::is_same<SimplifiedType<T>, T>::value>;
}

#endif
