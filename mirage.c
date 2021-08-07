#include "mirage.h"
#include "renderer.c"

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
}