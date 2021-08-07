
#include <string.h>
#include <sprintf.h>
#include <consts.h>
#include <stdio.h>
#include <graphics.h>

#include "mirage.c"

#define BACKBUFFER_W 80
#define BACKBUFFER_H 240

short __backbuffer[BACKBUFFER_W * BACKBUFFER_H];

void
MXBXProcessKeyboardInput(mxbx_button_state *newState, char isDown)
{
    newState->EndedDown = isDown;
    ++newState->HalfTransitionCount;
}

char
MXBXHandleEvents(mxbx_input *in)
{
    short vkp = is_key_pressed();
    short vkr = is_key_released();
    short keyCode = 0;
    if(!vkp && vkr || vkp && !vkr)
    {
        keyCode = vkp | vkr;
    }
    char shouldQuit = 0;
    if(vkp || vkr)
    {
        char isDown = vkp != 0;
        switch(keyCode)
        {
            case VK_LEFT_ARROW:
            {
                MXBXProcessKeyboardInput(&in->KeyboardController.StrafeLeft, isDown);
            } break;

            case VK_ESC:
            {
                shouldQuit = 1;
            } break;
        }
    }
    return shouldQuit;
}

void
MXBXBlitbackbuffer(mxbx_renderer *renderer)
{
	asm("push r1\npush r2\n push r3\nmov.w r1, 1024\nmov.w r2, %0\nmov.w r3, %1\nblit\npop r3\npop r2\npop r1\n"
		: /* No output */
		: "r" (renderer->Backbuffer), "g" (renderer->BackbufferW * renderer->BackbufferH * renderer->BackbufferStride));
}

int
main()
{
    init_stdio();
    video_mode(1);
    cls();
    char str[256];
    int startCounter = get_millis();

    mxbx_input inputs[2];
    mxbx_input *newInput = &inputs[0];
    mxbx_input *oldInput = &inputs[1];
    *newInput = (mxbx_input){0};
    *oldInput = (mxbx_input){0};

    mxbx_renderer renderer;
    renderer.BackbufferW = BACKBUFFER_W;
    renderer.BackbufferH = BACKBUFFER_H;
    renderer.BackbufferStride = 2;
    renderer.BackbufferPixelPerStride = 4;
    renderer.Backbuffer = __backbuffer;
    // GAME LOOP
    while(1)
    {
        mxbx_keyboard *oldKeyboardController = &oldInput->KeyboardController;
        mxbx_keyboard *newKeyboardController = &newInput->KeyboardController;
        *newKeyboardController = (mxbx_keyboard){0};

        for(int buttonIndex = 0;
            buttonIndex < ArrayCount(newKeyboardController->Buttons);
            ++buttonIndex)
        {
            newKeyboardController->Buttons[buttonIndex].EndedDown =
                oldKeyboardController->Buttons[buttonIndex].EndedDown;
        }

        if(MXBXHandleEvents(newInput))
        {
            cls();
            break;
        }

        UpdateAndRender(newInput, &renderer);
        draw(10, 20, RED, "aaaa");
        MXBXBlitbackbuffer(&renderer);

        int workCounter = get_millis() - startCounter;

        // NOTE(Jovan): Try retaining constant FPS
        if(workCounter < TARGET_MS_PER_FRAME)
        {
            delay(TARGET_MS_PER_FRAME - workCounter);
        }
        int endCounter = get_millis();
        sprintf(str, "MS Per frame: %d", endCounter - startCounter);
        draw(10, 10, RED, str);
        startCounter = endCounter;

        mxbx_input *tmp = newInput;
        newInput = oldInput;
        oldInput = tmp;
    }

    return 0;
}
