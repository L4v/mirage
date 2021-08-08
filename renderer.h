#ifndef RENDERER_H

#define BACKBUFFER_W 80
#define BACKBUFFER_H 240

i16 __emptybackbuffer[BACKBUFFER_W * BACKBUFFER_H];
i16 R_Backbuffer[BACKBUFFER_W * BACKBUFFER_H];

typedef struct mxbx_renderer_st
{
    i16 *Backbuffer;
    i32 BackbufferH;
    i32 BackbufferStride;
    i32 BackbufferW;
    i32 BackbufferPixelPerStride;
} mxbx_renderer;

#define RENDERER_H
#endif