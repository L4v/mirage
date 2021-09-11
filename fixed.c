#include "fixed.h"

q16
__fparsh(q16 x, i32 n)
{
	return x & FP_NEG_MASK ? (-1 << (32 - n)) | (x >> n) : x >> n;
}

q16
FPMul(q16 x, q16 y)
{
	q16 xs = x >= 0 ? x >> 8 : (x >> 8) | 0xFF000000;
	q16 ys = y >= 0 ? y >> 8 : (y >> 8) | 0xFF000000;
    return xs * ys;
}

q16
FPDiv(q16 x, q16 y)
{
	if(y == 0)
	{
		return 0;
	}
	//return __sdiv(x << 8, y) << 8;
	 return ((x << 8) / y) << 8;
}

// NOTE(Jovan): Extended integer range for less precision
q16
FPDiv79(q16 x, q16 y)
{
	if(y == 0)
	{
		return 0;
	}
	//return __sdiv(x << 7, y) << 9;
    return ((x << 7) / y) << 9;
}

q16
FPDiv511(q16 x, q16 y)
{
	if(y == 0)
	{
		return 0;
	}
	return ((x << 5) / y) << 11;
}

q16
__fpcos32s(q16 x)
{
	int c1 = 0xFFD8;
	int c2 = 0xFFFF8122;
	int c3 = 0x96B;
	int x2 = FPMul(x, x);
	int res = c1 + FPMul(x2, (c2 + FPMul(c3, x2))) + 39;
	return res;
}

q16
__fptan32s(q16 x)
{
	const q16 c1 = 0xFFFC6388;
	const q16 c2 = 0xFFFB62FE;
	q16 x2 = FPMul(x, x);
	return FPMul(x, FPDiv(c1, (c2 + x2)));
}

// https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c
q16
FPSqrt(q16 v) {
    u32 t, q, b, r;
    r = v;
    b = 0x40000000;
    q = 0;
    while( b > 0x40 )
    {
        t = q + b;
        if( r >= t )
        {
            r -= t;
            q = t + b; // equivalent to q += 2*b
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return q;
}

q16
FPCos(q16 x)
{
	// NOTE(Jovan): Skipping modulus, relying
	// on [0, 2PI] constraint
    // Check if < 0
	if(x & FP_NEG_MASK)
	{
		x = -x;
	}
	i32 quadrant = FPDiv(x, HalfPI) >> FP_SCALE;
	
	switch(quadrant)
	{
		case 0: return __fpcos32s(x);
		case 1: return -__fpcos32s(PI - x);
		case 2: return -__fpcos32s(x - PI);
		case 3: return __fpcos32s(TwoPI - x);
	}
	return 0;
}

q16
FPSin(q16 x)
{
	return FPCos(HalfPI - x);
}

q16
FPTan(q16 x)
{
	if (x == 0 || x == PI)
	{
		return 0;
	}
	i32 octant = FPDiv(x, QtrPI) >> FP_SCALE;
	switch(octant)
	{
		case 0: return __fptan32s(x);
		case 1: return FPDiv(0x10000, __fptan32s(HalfPI - x));
		case 2: return FPDiv(0xFFFF0000, __fptan32s(x - HalfPI));
		case 3: return -__fptan32s(PI - x);
		case 4: return __fptan32s(x - PI);
		case 5: return FPDiv(0x10000, __fptan32s(ThreeHalfPI - x));
		case 6: return FPDiv(0xFFFF0000, __fptan32s(x - ThreeHalfPI));
		case 7: return -__fptan32s(TwoPI - x);
	}
	return 0;
}
