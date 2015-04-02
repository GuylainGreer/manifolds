#include <boost/test/unit_test.hpp>
#include "functions/unary_minus.hh"
#include "functions/addition.hh"
#include "functions/std_functions.hh"
#include "functions/zero.hh"
#include "functions/polynomial.hh"
#include "functions/function_matrix.hh"
#include "functions/division.hh"
#include "functions/all_simplifications.hh"

BOOST_AUTO_TEST_CASE(simplification_ambiguities_test) {
  using namespace manifolds;

// commas in template parameter packs in macros are hard...
// hence this otherwise dumb-looking define
#define C ,

#define TESTCASE(code)                                                         \
  static_assert(Simplifies<code>::value, "Simplification failed")

  // Tests ambiguity of add_nadd and nadd_add and var_add
  TESTCASE(Addition<UnaryMinus<UnaryMinus<Sin> > C UnaryMinus<Sin> >);
  // Tests ambiguity of add_f_z and add_z_f and add_f_f and var_add
  TESTCASE(Addition<Zero C Zero>);
  // Tests ambiguity of add_f_p_1 and var_add
  TESTCASE(Addition<Sin C Polynomial<double C int_<1> > >);
  // Tests ambiguity of com_add and var_com
  TESTCASE(Composition<Addition<Sin C Cos> C Tan>);
  // Tests com_fm_fm and var_com
  TESTCASE(
      Composition<FunctionMatrix<int_<1> C int_<2> C Variable<0> C Variable<1> >
                      C FunctionMatrix<int_<1> C int_<2> C Sin C Cos> >);
  // Tests com_v_0_f and com_v_fm and var_com
  TESTCASE(Composition<
      Variable<0> C FunctionMatrix<int_<2> C int_<1> C Tan C Cos> >);
  // Tests com_mult_f and var_com
  TESTCASE(Composition<Multiplication<Variable<1> C Variable<0> > C
                           FunctionMatrix<int_<2> C int_<1> C Sin C Cos> >);
  // Tests com_p_1_fs and com_p1_p2 and var_com
  TESTCASE(
      Composition<Polynomial<double C int_<1> > C Polynomial<int C int_<4> > >);
  // Tests com_p_1_fs and and com_p_add_fs and var_com
  TESTCASE(Composition<Polynomial<int C int_<1> > C Addition<Sin C Sin> >);
  // Tests com_t_fm and var_com
  TESTCASE(
      Composition<Transpose C FunctionMatrix<int_<2> C int_<1> C Cos C Sin> >);
  // Tests com_z_f and var_com
  TESTCASE(Composition<Zero C Zero>);
  // Tests mult_div_dif and mult_div_f
  TESTCASE(Multiplication<Division<Sin C Cos> C Division<Cos C Sin> >);
  // Tests mult_f1_com_pow_fm_f2_f1 and var_mult and mult_um_f1_f2
  TESTCASE(Multiplication<UnaryMinus<Cos> C Composition<
      Pow C FunctionMatrix<int_<1> C int_<2> C Cos C UnaryMinus<Cos> > > >);
  // Tests add_p1_p2 and add_f_p_1
  TESTCASE(Addition<Polynomial<int C int_<12> > C Polynomial<int C int_<1> > >);
  // Tests um_um_f and um_f
  TESTCASE(UnaryMinus<UnaryMinus<Sin> >);
  // Tests mult_com_p1_fs_p2_1 and mult_com_p_f_f and
  // var_com_f1_fs_com_f2_fs
  TESTCASE(Multiplication<Composition<Polynomial<int C int_<2> > C Composition<
      Polynomial<int C int_<2> > C Sin> > C
                              Composition<Polynomial<int C int_<2> > C Sin> >);
  // Tests mult_z_f and var_f_com_p_f
  TESTCASE(
      Multiplication<Zero C Composition<Polynomial<int C int_<2> > C Zero> >);
  // Tests add_z_f and add_nadd
  TESTCASE(Addition<Zero C UnaryMinus<Zero> >);
  // Tests add_z_f and add_f_p_1
  TESTCASE(Addition<Zero C Polynomial<int C int_<1> > >);
  // Tests add_f_p_1 and add_f_f
  TESTCASE(Addition<Polynomial<int C int_<1> > C Polynomial<int C int_<1> > >);
  // Tests um_com_f_fs and um_f
  TESTCASE(UnaryMinus<Composition<Polynomial<int C int_<2> > C Sin> >);
  // Tests com_div_fs, var_com
  TESTCASE(Composition<Division<Sin C Cos> C Polynomial<int C int_<12> > >);
  // Tests mult_cos_cos and mult_f_f and var_mult
  TESTCASE(Multiplication<Cos C Cos>);
  // Tests var_com_f1_fs_com_f2_fs and var_add and add_f_f
  TESTCASE(Addition<Composition<Polynomial<int C int_<3> > C Cos> C
                        Composition<Polynomial<int C int_<3> > C Cos> >);
  // Tests var_com_f1_fs_com_f2_fs and var_com
  TESTCASE(Composition<Composition<Polynomial<int C int_<3> > C Cos> C
                           Composition<Polynomial<int C int_<3> > C Cos> >);
  // Tests var_grp and var_com_f1_fs_com_f2_fs
  TESTCASE(Group<Composition<Polynomial<int C int_<4> > C Cos> C
                     Composition<Polynomial<int C int_<4> > C Cos> >);
  // Tests var_com_f1_fs_com_f2_fs and var_mult and mult_f_f
  TESTCASE(Multiplication<Composition<Polynomial<int C int_<3> > C Cos> C
                              Composition<Polynomial<int C int_<3> > C Cos> >);
  // Tests var_com_f1_fs_com_f2_fs and add_com_p_f_f
  TESTCASE(
      Addition<Composition<Polynomial<int C int_<3> > C
                               Composition<Polynomial<int C int_<2> > C Sin> > C
                   Composition<Polynomial<int C int_<2> > C Sin> >);
  // Tests var_com_f1_fs_com_f2_fs and mult_com_p_f_f
  TESTCASE(Multiplication<Composition<Polynomial<int C int_<3> > C Composition<
      Polynomial<int C int_<2> > C Sin> > C
                              Composition<Polynomial<int C int_<2> > C Sin> >);
  // Tests add_f_z and nadd_add
  TESTCASE(Addition<UnaryMinus<Zero> C Zero>);
  // Tests com_z_z
  TESTCASE(Composition<Zero C Zero>);

  TESTCASE(Multiplication<Zero C IntegralPolynomial<1> >);
}
