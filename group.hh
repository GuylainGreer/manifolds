#ifndef MANIOLDS_FUNCTIONS_GROUP_HH
#define MANIOLDS_FUNCTIONS_GROUP_HH

namespace manifolds {
  template <class...Funcs>
  struct Group :
    Function<max<Funcs::input_dim...>::value,
	     sizeof...(Funcs)>
  {
    std::tuple<Funcs...> functions;

    Group(Funcs...fs):functions{fs...}{}

    template <std::size_t...is, class...Args>
    auto eval(std::integer_sequence<std::size_t,is...>,
	      Args...args) const
    {
      return std::make_tuple(std::get<is>(functions)(args...)...);
    }

    template <class ... Args>
    auto operator()(Args...args) const
    {
      return eval(std::index_sequence_for<Funcs...>(),
		  args...);
    }

    auto GetFunctions() const
    {
      return functions;
    }
  };

  template <class F, class ... Funcs>
  std::ostream & operator<<(std::ostream & s,
			    Group<F, Funcs...> g)
  {
    StreamVariadic("Group", g, s);
    return s;
  }

  template <class ... Functions>
  auto GetGroup(Functions ... fs)
  {
    return Group<Functions...>{fs...};
  }
}

#endif
