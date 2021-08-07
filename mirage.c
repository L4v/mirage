#include "mirage.h"

void
HandleInput(mxbx_input *input)
{
    char str[256];
    if(input->KeyboardController.StrafeLeft.EndedDown)
    {
        sprintf(str,"LEFT");
        draw(80, 80, YELLOW, str);
    }
}

void
UpdateAndRender(mxbx_input *input, mxbx_renderer *renderer)
{
    HandleInput(input);
    renderer->Backbuffer[0] = 0x1234;
    renderer->Backbuffer[renderer->BackbufferW * renderer->BackbufferH - 1] = 0x4321;
}