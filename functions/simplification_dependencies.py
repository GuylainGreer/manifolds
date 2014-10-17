#!/usr/bin/env python

import os, re, subprocess, sys

class Vertex:
    def __init__(self, pattern):
        self.pattern = pattern
        self.edges_out = []
        self.level = 0

def fix_levels(vertex):
    for l_c in vertex.edges_out:
        if l_c.level <= vertex.level:
            l_c.level = vertex.level + 1
            fix_levels(l_c)

def add_edge(preferred, less_preferred):
    preferred.edges_out.append(less_preferred)
    fix_levels(preferred)

var_add = Vertex('Addition<Functions...>')
com_add = Vertex('Composition<Addition<AFuncs...>,C>')
add_nadd = Vertex('Addition<F, UnaryMinus<F>>')
nadd_add = Vertex('Addition<UnaryMinus<F>, F>')
var_com = Vertex('Composition<Functions...>')
mult_div_div = Vertex('Multiplication<Division<N1,D1>,Division<N2,D2>>')
mult_div_f = Vertex('Multiplication<Division<N,D>,F>')
div_div_f = Vertex('Division<Division<N,D>,F>')
div_f_div = Vertex('Division<F, Division<N,D>>')
com_fm_fm = Vertex('Composition<FunctionMatrix<r1,c1,F1s...>,FunctionMatrix<r2,c2,F2s...>>')
com_v_fm = Vertex('Composition<Variable<i,a>,FunctionMatrix<FMFuncs...>>')
var_fm = Vertex('FunctionMatrix<r,c, Fs...>')
mult_f1_com_pow_fm_f2_f1 = Vertex('Multiplication<F, Composition<Pow,FunctionMatrix<r,c,F,P>>>')
mult_fm_fm = Vertex('Multiplication<FunctionMatrix<r1,c1,F1s...>,FunctionMatrix<r2,c2,F2s...>>')
fm_1_1 = Vertex('FunctionMatrix<int_<1>,int_<1>,F>')
var_grp = Vertex('Group<Functions...>')
var_mult = Vertex('Multiplication<Functions...>')
com_mult_f = Vertex('Composition<Multiplication<MFuncs...>,C>')
mult_f1_um_f2 = Vertex('Multiplication<F1, UnaryMinus<F2>>')
mult_um_f1_f2 = Vertex('Multiplication<UnaryMinus<F1>, F2>')
mult_add_add = Vertex('Multiplication<Addition<F1s...>, Addition<F2s...>>')
add_p1_p2 = Vertex('Addition<Polynomial<CoeffType1, int_<order1>>,Polynomial<CoeffType2, int_<order2>>>')
mult_p1_p2 = Vertex('Multiplication<Polynomial<CoeffType1,int_<order1>>,Polynomial<CoeffType2,int_<order2>>>')
com_p1_p2 = Vertex('Composition<Polynomial<CoeffType1,int_<order1>>,Polynomial<CoeffType2,int_<order2>>>')
um_p = Vertex('UnaryMinus<Polynomial<CoeffType, Order>>')
mult_com_p1_fs_p2_1 = Vertex('Multiplication<Composition<Polynomial<CoeffType1, Order>,Funcs...>,Polynomial<CoeffType2, int_<1>>>')
add_f_f = Vertex('Addition<A,A>')
mult_f_f = Vertex('Multiplication<A,A>')
var_com_f1_fs_com_f2_fs = Vertex('Variadic<Composition<Arg1, Args...>,Composition<Arg2, Args...>>')
add_com_p_f_f = Vertex('Addition<Composition<Polynomial<CType, int_<order>>, T>, T>')
mult_com_p_f_f = Vertex('Multiplication<Composition<Polynomial<CType, int_<order>>, T>, T>')
var_f_com_p_f = Vertex('Variadic<T, Composition<Polynomial<CType, int_<order>>, T>>')
com_p_1_fs = Vertex('Composition<Polynomial<CoeffType,int_<1>>,Functions...>')
add_f_p_1 = Vertex('Addition<T, Polynomial<C, int_<1>>>')
com_p_add_fs = Vertex('Composition<Polynomial<C,O>, Addition<Fs...>>')
com_t_fm = Vertex('Composition<Transpose,FunctionMatrix<r, c, Funcs...>>')
um_um_f = Vertex('UnaryMinus<UnaryMinus<F>>')
um_f = Vertex('UnaryMinus<F>')
com_v_0_f = Vertex('Composition<Variable<0,a>, F>')
add_f_z = Vertex('Addition<T,Zero>')
add_z_f = Vertex('Addition<Zero,T>')
mult_f_z = Vertex('Multiplication<T,Zero>')
mult_z_f = Vertex('Multiplication<Zero,T>')
com_z_f = Vertex('Composition<Zero, T>')
um_com_f_fs = Vertex('UnaryMinus<Composition<T,Fs...>>')
com_div_fs = Vertex('Composition<Division<N,D>, Fs...>')
mult_cos_cos = Vertex('Multiplication<Cos,Cos>')
com_f_z = Vertex('Composition<F,Zero>')
com_f_p_1 = Vertex('Composition<F, Polynomial<C,int_<1>>>')
add_ip_ip = Vertex('Addition<IPolynomial<>,IPolynomial<>>')
mult_ip_ip = Vertex('Multiplication<IPolynomial<>,IPolynomial<>>')
ip = Vertex('IntegralPolynomial<>')
var_p_ip = Vertex('Variadic<Polynomial<>,IPolynomial<>>')
var_ip_p = Vertex('Variadic<IPolynomial<>,Polynomial<>>')
mult_f_ip_1 = Vertex('Multiplication<T, IntegralPolynomial<1> >')
add_f_ip_1 = Vertex('Addition<T, IntegralPolynomial<>>')
div_div_div = Vertex('Division<Division<>,Division<>')

all_simplifications = [var_add, com_add, add_nadd, nadd_add,
                       var_com, mult_div_div, mult_div_f,
                       div_div_f, div_f_div, com_fm_fm,
                       com_v_fm, var_fm, mult_f1_com_pow_fm_f2_f1,
                       mult_fm_fm, fm_1_1, var_grp, var_mult,
                       com_mult_f, mult_f1_um_f2, mult_um_f1_f2,
                       mult_add_add, add_p1_p2, mult_p1_p2,
                       com_p1_p2, um_p, mult_com_p1_fs_p2_1,
                       add_f_f, mult_f_f, var_com_f1_fs_com_f2_fs,
                       add_com_p_f_f, mult_com_p_f_f,
                       var_f_com_p_f, com_p_1_fs, add_f_p_1,
                       com_p_add_fs, com_t_fm, um_um_f, um_f,
                       com_v_0_f, add_f_z, add_z_f, mult_f_z,
                       mult_z_f, com_z_f, um_com_f_fs, com_div_fs,
                       mult_cos_cos, com_f_z, com_f_p_1,
                       var_p_ip, mult_f_ip_1, add_f_ip_1,
                       var_ip_p, div_div_div]

add_edge(div_div_div,  div_div_f)
add_edge(div_div_div, div_f_div)
add_edge(var_p_ip, add_f_ip_1)
add_edge(mult_f_ip_1, var_p_ip)
add_edge(com_z_f, com_f_p_1)
add_edge(com_z_f, com_f_z)
add_edge(com_f_z, com_v_0_f)
add_edge(com_f_z, com_p_1_fs)
add_edge(com_f_z, com_mult_f)
add_edge(add_nadd, var_add)
add_edge(nadd_add, var_add)
add_edge(nadd_add, add_nadd)
add_edge(add_f_f, var_add)
add_edge(add_com_p_f_f, var_add)
add_edge(add_f_p_1, var_add)
add_edge(add_f_z, add_z_f)
add_edge(add_f_z, add_f_f)
add_edge(add_z_f, add_f_f)
add_edge(com_add, var_com)
add_edge(com_fm_fm, var_com)
add_edge(com_v_fm, var_com)
add_edge(com_mult_f, var_com)
add_edge(com_p1_p2, var_com)
add_edge(com_p_1_fs, var_com)
add_edge(com_p_1_fs, com_p1_p2)
add_edge(com_p_1_fs, com_p_add_fs)
add_edge(com_p_add_fs, var_com)
add_edge(com_t_fm, var_com)
add_edge(com_v_0_f, var_com)
add_edge(com_v_0_f, com_v_fm)
add_edge(com_z_f, var_com)
add_edge(mult_div_div, var_mult)
add_edge(mult_div_div, mult_div_f)
add_edge(mult_div_f, var_mult)
add_edge(mult_f1_com_pow_fm_f2_f1, var_mult)
add_edge(mult_fm_fm, var_mult)
add_edge(mult_f1_um_f2, var_mult)
add_edge(mult_um_f1_f2, var_mult)
add_edge(mult_um_f1_f2, mult_f1_um_f2)
add_edge(mult_um_f1_f2, mult_f_f)
add_edge(mult_f1_um_f2, mult_f_f)
add_edge(mult_add_add, var_mult)
add_edge(mult_p1_p2, var_mult)
add_edge(mult_com_p1_fs_p2_1, var_mult)
add_edge(mult_p1_p2, mult_f_f)
add_edge(mult_div_f, mult_f_f)
add_edge(mult_f_f, var_mult)
add_edge(var_com_f1_fs_com_f2_fs, var_mult)
add_edge(mult_com_p_f_f, var_mult)
add_edge(mult_f_z, var_mult)
add_edge(mult_z_f, var_mult)
add_edge(mult_z_f, mult_f_z)
add_edge(div_div_f, div_f_div)
add_edge(fm_1_1, var_fm)
add_edge(mult_f1_com_pow_fm_f2_f1, mult_um_f1_f2)
add_edge(add_p1_p2, add_f_p_1);
add_edge(um_um_f, um_f)
add_edge(mult_com_p1_fs_p2_1, mult_com_p_f_f)
add_edge(mult_com_p1_fs_p2_1, var_com_f1_fs_com_f2_fs)
add_edge(mult_z_f, var_f_com_p_f)
add_edge(add_z_f, add_nadd)
add_edge(add_z_f, add_f_p_1)
add_edge(add_f_p_1, add_f_f)
add_edge(um_com_f_fs, um_f)
add_edge(com_div_fs, var_com)
add_edge(mult_cos_cos, mult_f_f)
add_edge(mult_cos_cos, var_mult)
add_edge(var_com_f1_fs_com_f2_fs, var_add)
add_edge(var_com_f1_fs_com_f2_fs, var_com)
add_edge(var_grp, var_com_f1_fs_com_f2_fs)
add_edge(var_com_f1_fs_com_f2_fs, add_f_f)
add_edge(var_com_f1_fs_com_f2_fs, mult_f_f)
add_edge(var_com_f1_fs_com_f2_fs, add_com_p_f_f)
add_edge(var_com_f1_fs_com_f2_fs, mult_com_p_f_f)
add_edge(add_f_z, nadd_add)


#This is hacky, but too bad
def var_name(x):
    y = None
    for y in globals():
        try:
            if globals()[y].pattern == x and y != 'x' and y != 'y':
                return y
        except:
            pass
    return None

def all_var_names():
    y = None
    results = []
    for y in globals():
        try:
            if len(globals()[y].pattern) > 0:
                results.append(y)
        except:
            pass
    return results

max_level = max(map(lambda x: x.level, all_simplifications))
comp_filename = lambda x : 'simplification_comparison_' + str(x) + '.txt'

if '-print' in sys.argv:
    for i in range(max_level+1):
        print 'level', i
        level_is = [x.pattern for x in all_simplifications if x.level == i]
        level_is.sort()
        for s in level_is:
            print ' (' + var_name(s) + ',' + ' ' * (25 - len(var_name(s))), s + ')'
        print
if '-compare' in sys.argv:
    max_comp = -1
    exp = re.compile('simplification_comparison_([0-9]+).txt')
    for f in os.listdir('.'):
        m = re.match(exp, f)
        if m:
            max_comp = max([max_comp, int(m.group(1))])
    
    lines = [str(s.level) + ' ' + s.pattern \
             for s in all_simplifications]
    lines.sort()
    f = open(comp_filename(max_comp+1), 'w+')
    for l in lines:
        f.write(l)
        f.write('\n')
    f.write('num levels: ' + str(max_level+1) + '\n')
    f.close()
    if max_comp != -1:
        r = ''
        try:
            r = subprocess.check_output(['diff',
                                         comp_filename(max_comp),
                                         comp_filename(max_comp+1)])
        except subprocess.CalledProcessError, e:
            r = e.output
        if len(r) == 0:
            os.remove(comp_filename(max_comp+1))
            print 'No differences found'
        else:
            print r
if '-apply' in sys.argv:
    all_files = [x for x in os.listdir('.') if \
                 os.path.splitext(x)[1] in \
                 ['.hh', '.cpp', '.hpp', '.h']]
    all_vars = [('/*'+x+'*/',globals()[x].level) \
                for x in all_var_names()]
    key = 'static const int num_simplification_levels = '
    all_vars.append((key, max_level+1))
    for f in all_files:
        fd = open(f)
        orig_lines = [x for x in fd.readlines()]
        fd.close()
        new_lines = [x for x in orig_lines]
        for v in all_vars:
            next_lines = []
            for l in new_lines:
                i1 = l.find(v[0])
                if i1 == -1:
                    next_lines.append(l)
                    continue
                i1 += len(v[0])
                i2 = i1 + 1
                while l[i2].isdigit():
                    i2 += 1
                priority = int(l[i1:i2])
                if priority != v[1]:
                    nl = l[:i1] + str(v[1]) + l[i2:]
                    next_lines.append(nl)
                else:
                    next_lines.append(l)
            new_lines = next_lines
        file_changed = False
        for l1, l2 in zip(orig_lines, new_lines):
            if l1 != l2:
                print 'File ', f, 'changed'
                file_changed = True
                break
        if file_changed:
            fd = open(f, 'w')
            for l in new_lines:
                fd.write(l)
            fd.close()
