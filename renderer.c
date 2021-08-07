#include "renderer.h"

void
__drawpixel(short *backbuffer, int bufferW, int bufferH, int bufferPPS, int x, int y, unsigned short color)
{
	if(x < 0 || y < 0 || x >= bufferW * bufferPPS || y >= bufferH)
	{
		return;
	}
	int location = (x >> 2) + y * bufferW;
	short* p = backbuffer + location;
	short xOffset = (x % 4) * 4;
	unsigned short xOffsetMask = 0xF000 >> xOffset;
	unsigned short pColor = color & xOffsetMask;
	unsigned short bgColor = (~xOffsetMask) & *p;
	*p = pColor | bgColor;
}

// NOTE(Jovan): Bresenham's line drawing algorithm
void
__drawline(short *backbuffer, int bufferW, int bufferH, int bufferPPS, int x0, int y0, int x1, int y1, unsigned short color)
{
	if(x0 < 0 || x0 >= bufferW * bufferPPS || y0 < 0 || y0 >= bufferH
	|| x1 < 0 || x1 >= bufferW * bufferPPS || y0 < 0 || y0 >= bufferH)
	{
		return;
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int absdx = ABS(dx);
	int absdy = ABS(dy);
	int x = x0;
	int y = y0;
	__drawpixel(backbuffer, bufferW, bufferH, bufferPPS, x, y, color);
	// NOTE(Jovan): Slope < 1
	if(absdx > absdy)
	{
		int d = 2 * absdy - absdx;
		for(int i = 0;
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
		int d = 2 * absdx - absdy;
		for(int i = 0;
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
__drawrect(short *backbuffer, int bufferW, int bufferH, int bufferPPS, int x, int y, int w, int h, unsigned short color) // x = 2 y = 1 w = 3 h = 1 c = 0x1234
{
	short* p = backbuffer;
	p += (x >> 2) + y * bufferW;
	short* pRow;
	short xRemainder = x % 4; // 2

	if(w < 4)
	{
		unsigned short startMask = 0xFFFF << (4 - w) * 4; // 0xFFF0
		startMask = startMask >> xRemainder * 4; // 0x00FF
		unsigned short startBGMask = ~startMask; // 0xFF00
		unsigned short startColor = startMask & (color >> xRemainder * 4); // 0x00FF & 0x0012
		short trailLength = w - 4 + xRemainder; // 1
		unsigned short trailingMask = 0xFFFF << (4 - trailLength) * 4; // 0xF000
		unsigned short trailingBGMask = ~trailingMask; // 0x0FFF
		unsigned short trailingColor = (color << (4 - xRemainder) * 4) & trailingMask; // 0x234
		for(int i = 0;
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

	int columns = (xRemainder + w) >> 2;
	short columnRemainder = (xRemainder + w) % 4;

	short startMask = (unsigned short)0xFFFF >> xRemainder * 4;
	short startBGMask = ~startMask;
	short startColor = color >> xRemainder * 4;
	short trailingOffset = (4 - columnRemainder) * 4;
	short trailingMask = (unsigned short)0xFFFF << trailingOffset;
	short trailingBGMask = ~trailingMask;
	short innerColor = (color << (4 - xRemainder) * 4) | (color >> xRemainder * 4);
	short trailingColor = trailingMask & innerColor;

	for(int i = 0;
		i < h;
		++i)
	{
		pRow = p;
		*(pRow++) = startColor | (startBGMask & *pRow);
		for(int j = 1;
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
DrawPixel(mxbx_renderer *renderer, int x, int y, unsigned short color)
{
    __drawpixel(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH, renderer->BackbufferPixelPerStride, x, y, color);
}

void
DrawLine(mxbx_renderer *renderer, int x0, int y0, int x1, int y1, unsigned short color)
{
    __drawline(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH,
        renderer->BackbufferPixelPerStride, x0, y0, x1, y1, color);
}

void
DrawRect(mxbx_renderer *renderer, int x, int y, int w, int h, unsigned short color)
{
    __drawrect(renderer->Backbuffer, renderer->BackbufferW, renderer->BackbufferH,
        renderer->BackbufferPixelPerStride, x, y, w, h, color);
}