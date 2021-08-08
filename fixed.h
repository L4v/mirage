#ifndef FIXED_H

#include "intrinsics.h"

#define FP_SCALE 16
#define FP_NEG_MASK 0xF0000000

typedef i32 q16;

q16 const PI = 0x3243F;
q16 const TwoPI = 0x6487E;
q16 const HalfPI = 0x1921F;
q16 const QtrPI = 0xC90F;
q16 const ThreeHalfPI = 0x96CBE;
q16 const DegInRad = 0x477;


#define FIXED_H
#endif