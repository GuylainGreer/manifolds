#ifndef MANIFOLDS_FUNCTIONS_MULTI_DIMENSIONAL_MATRIX_HH
#define MANIFOLDS_FUNCTIONS_MULTI_DIMENSIONAL_MATRIX_HH

#include <array>
#include <algorithm>
#include <iterator>
#include "function.hh"
#include "tuple_util.hh"

namespace manifolds {
template <int n, int... ns> struct product {
  static const int value = n * product<ns...>::value;
  typedef int_<value> type;
};

template <int n> struct product<n> {
  static const int value = n;
  typedef int_<value> type;
};

template <class CoeffType, int... sizes> struct MultiMatrix {
  static const int dimensions = sizeof...(sizes);
  static const int num_elements = product<sizes...>::value;
  std::array<CoeffType, num_elements> coeffs;
  typedef CoeffType CoefficientType;

  template <int N> struct dimension {
    typedef typename std::tuple_element<N, tuple<int_<sizes>...> >::type type;
    static const int value = type::value;
  };

  static std::array<int, sizeof...(sizes)> Dimensions() {
    return { { sizes... } };
  }

  static auto Dimension(int n) {
    static const int a[] = { sizes... };
    return a[n];
  }

  template <class... Args>
  MultiMatrix(Args... args)
      : coeffs{ {(CoeffType)args... } } {}

  template <class T, class = typename std::enable_if<array_size<T>::value ==
                                                     sizeof...(sizes)>::type>
  CoeffType Coeff(const T &indices) const {
    static const int sizes_a[] = { sizes... };
    unsigned index = 0;
    for (unsigned i = 0; i < sizeof...(sizes); i++) {
      index = (index * sizes_a[i] + indices[i]);
    }
    return coeffs[index];
  }

  template <class T, class = typename std::enable_if<array_size<T>::value ==
                                                     sizeof...(sizes)>::type>
  CoeffType &Coeff(const T &indices) {
    static const int sizes_a[] = { sizes... };
    unsigned index = 0;
    for (unsigned i = 0; i < sizeof...(sizes); i++) {
      index = (index * sizes_a[i] + indices[i]);
    }
    return coeffs[index];
  }

  template <class Index, class... Indices,
            class = typename std::enable_if<
                std::is_arithmetic<Index>::type::value>::type>
  CoeffType Coeff(Index i, Indices... indices) const {
    static_assert(sizeof...(Indices) + 1 == sizeof...(sizes),
                  "Must index into multi-matrix with the same "
                  "number of indices as the "
                  "matrix has dimensions");
    std::array<unsigned, sizeof...(Indices) + 1> indices_a = {
      {(unsigned)i, (unsigned)indices... }
    };
    return Coeff(indices_a);
  }

  template <class Index, class... Indices,
            class = typename std::enable_if<
                std::is_arithmetic<Index>::type::value>::type>
  CoeffType &Coeff(Index i, Indices... indices) {
    static_assert(sizeof...(Indices) + 1 == sizeof...(sizes),
                  "Must index into multi-matrix with the same "
                  "number of indices as the "
                  "matrix has dimensions");
    std::array<unsigned, sizeof...(Indices) + 1> indices_a = {
      {(unsigned)i, (unsigned)indices... }
    };
    return Coeff(indices_a);
  }
};

template <class... MMatrices> struct MultiMatrixGroup {
  tuple<MMatrices...> matrices;
  static const int dimensions = sum<MMatrices::dimensions...>::value;
  typedef typename std::common_type<
      typename MMatrices::CoefficientType...>::type CoefficientType;

  template <int N>
  using dim_helper = typename std::tuple_element<N, tuple<MMatrices...> >::type;

  template <int N, int index = 0, bool = (N < dim_helper<index>::dimensions)>
  struct dimension
      : std::tuple_element<
            index, tuple<MMatrices...> >::type::template dimension<N>::type {};

  template <int N, int index>
  struct dimension<N, index,
                   false> : dimension<N - dim_helper<index>::dimensions,
                                      index + 1> {};

  static auto Dimensions() {
    auto a = { MMatrices::Dimensions()... };
    std::array<int, dimensions> b;
    auto bb = std::begin(b);
    for (auto aa : a)
      bb = std::copy(std::begin(aa), std::end(aa), bb);
    return b;
  }

  static auto Dimension(int n) {
    auto d = Dimensions();
    return d[n];
  }

  MultiMatrixGroup(MMatrices... mms) : matrices(mms...) {}
  MultiMatrixGroup(tuple<MMatrices...> t) : matrices(t) {}

  template <class T, class U, class V>
  auto GetCoeff(V, T, U, int_<sizeof...(MMatrices)>) const {
    return 1;
  }

  template <
      int offset, int... individual_indices, std::size_t num_indices, int index,
      class = typename std::enable_if<index != sizeof...(MMatrices)>::type>
  auto GetCoeff(std::array<int, num_indices> a,
                std::integer_sequence<int, individual_indices...>, int_<offset>,
                int_<index>) const {
    static const int dims =
        std::tuple_element<index, tuple<MMatrices...> >::type::dimensions;
    return GetCoeff(a, std::make_integer_sequence<int, dims>(),
                    int_<offset + dims>(), int_<index + 1>()) *
           std::get<index>(matrices).Coeff(a[individual_indices + offset]...);
  }

  template <class T, class = typename std::enable_if<array_size<T>::value ==
                                                     dimensions>::type>
  auto Coeff(const T &indices) const {
    std::array<int, array_size<T>::value> a;
    std::copy(std::begin(indices), std::end(indices), a.begin());
    return GetCoeff(
        a, std::make_integer_sequence<
               int,
               std::tuple_element<0, tuple<MMatrices...> >::type::dimensions>(),
        int_<0>(), int_<0>());
  }

  template <class... Indices> auto Coeff(Indices... indices) const {
    std::array<int, sizeof...(Indices)> a{ { int(indices)... } };
    return GetCoeff(
        a, std::make_integer_sequence<
               int,
               std::tuple_element<0, tuple<MMatrices...> >::type::dimensions>(),
        int_<0>(), int_<0>());
  }

  auto GetFunctions() const { return matrices; }
};

template <class CoeffType1, int... sizes1, class CoeffType2, int... sizes2>
auto operator*(MultiMatrix<CoeffType1, sizes1...> a,
               MultiMatrix<CoeffType2, sizes2...> b) {
  return MultiMatrixGroup<decltype(a), decltype(b)>(a, b);
}

template <class CoeffType, int... sizes, class... MMatrices>
auto operator*(MultiMatrixGroup<MMatrices...> b,
               MultiMatrix<CoeffType, sizes...> a) {
  typedef MultiMatrixGroup<MMatrices..., MultiMatrix<CoeffType, sizes...> >
  type;

  return type{ insert_element<sizeof...(MMatrices)>(b.GetFunctions(), a) };
}

template <class CoeffType, int... sizes, class... MMatrices>
auto operator*(MultiMatrix<CoeffType, sizes...> a,
               MultiMatrixGroup<MMatrices...> b) {
  typedef MultiMatrixGroup<MultiMatrix<CoeffType, sizes...>, MMatrices...> type;

  return type{ insert_element<0>(b.GetFunctions(), a) };
}

template <class... MMatrices1, class... MMatrices2>
auto operator*(MultiMatrixGroup<MMatrices1...> a,
               MultiMatrixGroup<MMatrices2...> b) {
  typedef MultiMatrixGroup<MMatrices1..., MMatrices2...> type;

  return type{ tuple_cat(a.GetFunctions(), b.GetFunctions()) };
}

template <int... dims, class... Coeffs> auto GetMultiMatrix(Coeffs... coeffs) {
  return MultiMatrix<typename std::common_type<Coeffs...>::type, dims...>(
      coeffs...);
}
}

#endif
