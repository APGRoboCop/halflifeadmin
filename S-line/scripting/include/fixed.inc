/* Fixed point arithmetic
 *
 * (c) Copyright 1998-2002, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined _fixed_included
  #endinput
#endif
#define _fixed_included

enum fround_method {
  fround_round,
  fround_floor,
  fround_ceil
}

native fixed:fixed(value);
native fixed:fixedstr(const string[]);
native fixed:fmul(fixed:oper1, fixed:oper2);
native fixed:fdiv(fixed:dividend, fixed:divisor);
native fixed:ffract(fixed:value);
native       fround(fixed:value, fround_method:method=fround_round);

#pragma rational fixed(3)

/* user defined operators */
native fixed:operator*(fixed:oper1, fixed:oper2) = fmul;
native fixed:operator/(fixed:oper1, fixed:oper2) = fdiv;

stock fixed:operator++(fixed:oper)
    return oper+fixed(1);

stock fixed:operator--(fixed:oper)
    return oper-fixed(1);

stock fixed:operator*(fixed:oper1, oper2)
    return _:oper1 * oper2;                     /* "*" is commutative */

stock fixed:operator/(fixed:oper1, oper2)
    return _:oper1 / oper2;

stock fixed:operator/(oper1, fixed:oper2)
    return fdiv(fixed(oper1), oper2);

stock fixed:operator+(fixed:oper1, oper2)
    return oper1 + fixed(oper2);                /* "+" is commutative */

stock fixed:operator-(fixed:oper1, oper2)
    return oper1 - fixed(oper2);

stock fixed:operator-(oper1, fixed:oper2)
    return fixed(oper1) - oper2;

/* forbidden operations */
forward operator%(fixed:oper1, fixed:oper2);
forward operator%(fixed:oper1, oper2);
forward operator%(oper1, fixed:oper2);

