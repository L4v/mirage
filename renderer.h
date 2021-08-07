#ifndef RENDERER_H

typedef struct mxbx_renderer_st
{
    short *Backbuffer;
    int BackbufferH;
    int BackbufferStride;
    int BackbufferW;
    int BackbufferPixelPerStride;
} mxbx_renderer;

#define RENDERER_H
#endif