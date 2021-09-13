#include "renderer.h"

void
__drawpixel(i16 *backbuffer, i32 bufferW, i32 bufferH, i32 bufferPPS, i32 x, i32 y, u16 color)
{
	if(x < 0 || y < 0 || x >= bufferW * bufferPPS || y >= bufferH)
	{
		return;
	}
	i32 location = (x >> 2) + y * bufferW;
	i16* p = backbuffer + location;
	i16 xOffset = (x % 4) * 4;
	u16 xOffsetMask = 0xF000 >> xOffset;
	u16 pColor = color & xOffsetMask;
	u16 bgColor = (~xOffsetMask) & *p;
	*p = pColor | bgColor;
}

// NOTE(Jovan): Extremely Fast Line Algorithm
// http://www.edepot.com/algorithm.html
void
__efla(i16 *backbuffer, i32 bufferW, i32 bufferH, i32 bufferPPS, i32 x0, i32 y0, i32 x1, i32 y1, u16 color)
{
	u8 yLonger = 0;
	i32 shortLen = y1 - y0;
	i32 longLen = x1 - x0;
	if (ABS(shortLen) > ABS(longLen))
	{
		i32 swap = shortLen;
		shortLen = longLen;
		longLen = swap;				
		yLonger = 1;
	}
	i32 decInc;
	if (longLen == 0)
	{
		decInc=0;
	}
	else
	{
		decInc = (shortLen << FP_SCALE) / longLen;
	}

	if (yLonger) 
	{
		if (longLen > 0)
		{
			longLen += y0;
			for (i32 j = 0x8000 + (x0 << FP_SCALE); y0 <= longLen; ++y0)
			{
				__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, j >> FP_SCALE, y0, color);
				j+=decInc;
			}
			return;
		}
		longLen += y0;
		for (i32 j = 0x8000 + (x0 << 16); y0 >= longLen; --y0)
		{
			__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, j >> FP_SCALE, y0, color);
			j -= decInc;
		}
		return;	
	}

	if (longLen>0)
	{
		longLen += x0;
		for (i32 j = 0x8000 + (y0 << FP_SCALE); x0 <= longLen; ++x0)
		{
			__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x0, j >> FP_SCALE, color);
			j += decInc;
		}
		return;
	}
	longLen += x0;
	for (int j= 0x8000 + (y0 << FP_SCALE); x0 >= longLen; --x0)
	{
		__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x0, j >> FP_SCALE, color);
		j -= decInc;
	}
}

// NOTE(Jovan): Bresenham's line drawing algorithm
void
__bresenhamline(i16 *backbuffer, i32 bufferW, i32 bufferH, i32 bufferPPS, i32 x0, i32 y0, i32 x1, i32 y1, u16 color)
{
	if(x0 < 0 || x0 >= bufferW * bufferPPS || y0 < 0 || y0 >= bufferH
	|| x1 < 0 || x1 >= bufferW * bufferPPS || y1 < 0 || y1 >= bufferH)
	{
		return;
	}
	i32 dx = x1 - x0;
	i32 dy = y1 - y0;
	i32 absdx = ABS(dx);
	i32 absdy = ABS(dy);
	i32 x = x0;
	i32 y = y0;
	__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x, y, color);
	// NOTE(Jovan): Slope < 1
	if(absdx > absdy)
	{
		i32 d = 2 * absdy - absdx;
		for(i32 i = 0;
			i < absdx;
			++i)
		{
			x = dx < 0 ? x - 1 : x + 1;
			if(d < 0)
			{
				d += 2 * absdy;
			}
			else
			{
				y = dy < 0 ? y - 1 : y + 1;
				d += 2 * absdy - 2 * absdx;
			}
			__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x, y, color);
		}
	}
	else
	{
		// NOTE(Jovan): Slope >= 1
		i32 d = 2 * absdx - absdy;
		for(i32 i = 0;
			i < absdy;
			++i)
		{
			y = dy < 0 ? y - 1 : y + 1;
			if(d < 0)
			{
				d += 2 * absdx;
			}
			else
			{
				x = dx < 0 ? x - 1 : x + 1;
				d += 2 * absdx - 2 * absdy;
			}
			__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x, y, color);
		}
	}
}

void
__drawrect(i16 *backbuffer, i32 bufferW, i32 bufferH, i32 bufferPPS, i32 x, i32 y, i32 w, i32 h, u16 color) // x = 2 y = 1 w = 3 h = 1 c = 0x1234
{
	i16* p = backbuffer;
	p += (x >> 2) + y * bufferW;
	i16* pRow;
	i16 xRemainder = x % 4; // 2

	if(w < 4)
	{
		u16 startMask = 0xFFFF << (4 - w) * 4; // 0xFFF0
		startMask = startMask >> xRemainder * 4; // 0x00FF
		u16 startBGMask = ~startMask; // 0xFF00
		u16 startColor = startMask & (color >> xRemainder * 4); // 0x00FF & 0x0012
		i16 trailLength = w - 4 + xRemainder; // 1
		u16 trailingMask = 0xFFFF << (4 - trailLength) * 4; // 0xF000
		u16 trailingBGMask = ~trailingMask; // 0x0FFF
		u16 trailingColor = (color << (4 - xRemainder) * 4) & trailingMask; // 0x234
		for(i32 i = 0;
			i < h;
			++i)
		{
			pRow = p;
			*(pRow++) = startColor | (startBGMask & *pRow); // 0x0200 | (0x0000)
			if(trailLength > 0)
			{
				*(pRow++) = trailingColor | (trailingBGMask & *pRow);
			}
			p += bufferW;
		}
		return;
	}

	i32 columns = (xRemainder + w) >> 2;
	i16 columnRemainder = (xRemainder + w) % 4;

	i16 startMask = (u16)0xFFFF >> xRemainder * 4;
	i16 startBGMask = ~startMask;
	i16 startColor = color >> xRemainder * 4;
	i16 trailingOffset = (4 - columnRemainder) * 4;
	i16 trailingMask = (u16)0xFFFF << trailingOffset;
	i16 trailingBGMask = ~trailingMask;
	i16 innerColor = (color << (4 - xRemainder) * 4) | (color >> xRemainder * 4);
	i16 trailingColor = trailingMask & innerColor;

	for(i32 i = 0;
		i < h;
		++i)
	{
		pRow = p;
		*(pRow++) = startColor | (startBGMask & *pRow);
		for(i32 j = 1;
			j < columns;
			++j)
		{
			*(pRow++) = innerColor;
		}
		if(trailingOffset)
		{
			*pRow = trailingColor | (trailingBGMask & *pRow);
		}
		p += bufferW;
	}
}

void
DrawPixel(mxbx_renderer *renderer, i32 x, i32 y, u16 color)
{
    __drawpixel(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH, renderer->BackbufferPixelPerStride, x, y, color);
}

void
DrawLine(mxbx_renderer *renderer, i32 x0, i32 y0, i32 x1, i32 y1, u16 color)
{
    // __bresenhamline(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH,
    //     renderer->BackbufferPixelPerStride, x0, y0, x1, y1, color);
	__efla(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH,
		renderer->BackbufferPixelPerStride, x0, y0, x1, y1, color);
}

void
DrawRect(mxbx_renderer *renderer, i32 x, i32 y, i32 width, i32 height, u16 color)
{
	if(x < 0 || y < 0 || x >= 320 || y >= 240)
	{
		return;
	}
	int w = width;
	int h = height;
	int sx = x >> 2;
	if(sx + w >= 80)
	{
		w = sx + w - 80 - 1;
	}

	if(y + h >= 240)
	{
		h = y + h - 240 - 1;
	}

    __drawrect(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH,
        renderer->BackbufferPixelPerStride, x, y, w, h, color);
}

void
ClearBackbuffer()
{
	for(u32 i = 0; i < BACKBUFFER_H * BACKBUFFER_W; ++i)
		R_Backbuffer[i] = 0;
	// asm("push r1\npush r2\n push r3\nmov.w r1, %0\nmov.w r2, %1\nmov.w r3, %2\nblit\npop r3\npop r2\npop r1\n"
	// 	: /* No output */
	// 	: "i" (R_Backbuffer), "r" (__emptybackbuffer), "i" (BACKBUFFER_W * BACKBUFFER_H * 2));
}