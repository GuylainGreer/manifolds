#ifndef MANIFOLDS_FUNCTIONS_MULTI_MATRIX_REDUCTION_HH
#define MANIFOLDS_FUNCTIONS_MULTI_MATRIX_REDUCTION_HH

#include "multi_matrix.hh"
#include "function.hh"

#include <boost/mpl/find.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/end.hpp>
#include <utility>

namespace manifolds
{
  template <int n1, int n2>
  struct ReduxPair
  {
    static const int first = n1;
    static const int second = n2;
  };

  template <class ... ReductionPairs>
  struct Reduction : Function
  {
    template <class Arg, class Array, class ... Indices,
	      class = typename std::enable_if<
		(sizeof...(Indices) == Arg::dimensions)
		>::type>
    static auto CoeffFromArray(void*,Arg arg, Array a,
			Indices...indices)
    {
      return arg.Coeff(indices...);
    }

    template <class Arg, class Array, class ... Indices,
	      class = typename std::enable_if<
		(sizeof...(Indices) != Arg::dimensions)
		>::type>
    static auto CoeffFromArray(int*,Arg arg, Array a,
			Indices ... indices)
    {
      return CoeffFromArray(arg, a, indices...,
			    a[sizeof...(Indices)]);
    }

    template <class Vector, int index>
    using is_not_in =
      typename boost::is_same<
      typename boost::mpl::find<
	Vector, int_<index>
	>::type,
      typename boost::mpl::end<Vector>::type
      >::type;

    template <class MMatrix,
	      class Skips,
	      class IndicesTuple = std::tuple<>,
	      int index = 0,
	      bool = MMatrix::dimensions == index>
    struct output_type
    {
      typedef int_<
	MMatrix::template dimension<index>::value> dim;
      typedef typename std::conditional<
	is_not_in<Skips, index>::value,
	decltype(push_back(std::declval<IndicesTuple>(),
			   dim())),
	IndicesTuple
	>::type next_tuple;

      typedef typename output_type<
	MMatrix,
	Skips,
	next_tuple,
	index+1
	>::type type;
    };

    template <class MMatrix, class Skips,
	      int index, int ... dimensions>
    struct output_type<
      MMatrix, Skips, std::tuple<int_<dimensions>...>,
      index, true>
    {
      typedef MultiMatrix<
	typename MMatrix::CoefficientType,
	dimensions...> type;
    };

    template <class Array>
    static bool inc_pair(Array & a, int b, int c, int dim)
    {
      ++a[b];
      ++a[c];
      return a[b] == dim;
    }

    template <class Arg>
    auto operator()(Arg arg) const
    {
      static const bool all_good =
	and_<bool_<
	       Arg::template dimension<
		 ReductionPairs::first>::value ==
	       Arg::template dimension<
		 ReductionPairs::second>::value>...>::value;
      static_assert(all_good, "Can only contract indices "
		    "of the same dimension");
      static std::array<std::pair<int,int>,
			sizeof...(ReductionPairs)> reduxes =
	{std::make_pair(ReductionPairs::first,
			ReductionPairs::second)...};
      int in_indices[Arg::dimensions] = {};
      typename output_type<
	Arg,
	boost::mpl::vector<
	  int_<ReductionPairs::first>...,
	  int_<ReductionPairs::second>...
	  >
	>::type result;
      int out_indices[decltype(result)::dimensions] = {};
      while(true)
	{
	  while(true)
	    {
	      result.Coeff(out_indices) +=
		arg.Coeff(in_indices);
	      unsigned index = 0;
	      while(inc_pair(in_indices, reduxes[index].first,
			     reduxes[index].second,
			     arg.Dimension(reduxes[index].second)))
		{
		  in_indices[reduxes[index].first] = 0;
		  in_indices[reduxes[index].second] = 0;
		  if(++index == sizeof...(ReductionPairs))
		    {
		      goto output_index_done;
		    }
		}
	    }
	output_index_done:
	  int index = 0;
	  while(true)
	    {
	      if(std::find_if(reduxes.begin(), reduxes.end(), 
			   [index](auto x)
			      {return x.first == index ||
			       x.second == index;}) ==
		 reduxes.end())
		{
		  if(++in_indices[index] == arg.Dimension(index))
		    {
		      int smaller_ones = 0;
		      for(auto x : reduxes)
			{
			  smaller_ones +=
			    (x.first < index) +
			    (x.second < index);
			}
		      in_indices[index] =
			out_indices[index-smaller_ones] = 0;
		      if(++index == (int)Arg::dimensions)
			return result;
		    }
		  else
		    {
		      int smaller_ones = 0;
		      for(auto x : reduxes)
			{
			  smaller_ones +=
			    (x.first < index) +
			    (x.second < index);
			}
		      ++out_indices[index-smaller_ones];
		      break;
		    }
		}
	      else if(++index == (int)Arg::dimensions)
		return result;
	    }
	}
    }
  };
}

#endif
