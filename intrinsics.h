#ifndef INTRINSICS_H

#include "types.h"

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))
#define ABS(x) (((x) < 0) ? -(x) : (x))

/*
   Copyright stuff

   Use of this program, for any purpose, is granted the author,
   Ian Kaplan, as long as this copyright notice is included in
   the source code or any source code derived from this program.
   The user assumes all responsibility for using this code.

   Ian Kaplan, October 1996

*/
u32
__udiv(u32 dividend, u32 divisor)
{
    u32 t, num_bits;
    u32 q, bit, d;
    i32 i;

    i32 remainder = 0;
    i32 quotient = 0;

    if (divisor == 0 || divisor > dividend)
    {
        return 0;
    }

    if (divisor == dividend)
    {
        return 1;
    }

    num_bits = 32;

    while (remainder < divisor)
    {
        bit = (dividend & 0x80000000) >> 31;
        remainder = (remainder << 1) | bit;
        d = dividend;
        dividend = dividend << 1;
        num_bits--;
    }

    dividend = d;
    remainder = remainder >> 1;
    num_bits++;

    for (i = 0;
        i < num_bits;
        ++i)
    {
        bit = (dividend & 0x80000000) >> 31;
        remainder = (remainder << 1) | bit;
        t = remainder - divisor;
        q = !((t & 0x80000000) >> 31);
        dividend = dividend << 1;
        quotient = (quotient << 1) | q;
        if (q)
        {
            remainder = t;
        }
    }
    return quotient;
}

i32 
__sdiv(i32 dividend, i32 divisor)
{
    u32 dend, dor;
    u32 q, r;
    i32 quotient;

    dend = ABS(dividend);
    dor  = ABS(divisor);
    q = __udiv(dend, dor);

    quotient = q;
    if (dividend < 0)
    {
        if (divisor > 0)
        {
            quotient = -q;
        }
    }
    else
    {
        if (divisor < 0)
        {
            quotient = -q;
        }
    }
    return quotient;
}

#define INTRINSICS_H
#endif