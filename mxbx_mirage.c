
#include <string.h>
#include <sprintf.h>
#include <consts.h>
#include <stdio.h>
#include <graphics.h>
#include <floatimpl.h>

#include "mirage.c"

void
MXBXProcessKeyboardInput(mxbx_button_state *newState, i8 isDown)
{
    newState->EndedDown = isDown;
    ++newState->HalfTransitionCount;
}

i8
MXBXHandleEvents(mxbx_input *in)
{
    i16 vkp = is_key_pressed();
    i16 vkr = is_key_released();
    i16 keyCode = 0;
    if(!vkp && vkr || vkp && !vkr)
    {
        keyCode = vkp | vkr;
    }
    i8 shouldQuit = 0;
    if(vkp || vkr)
    {
        i8 isDown = vkp != 0;
        switch(keyCode)
        {
            case VK_UP_ARROW:
            {
                MXBXProcessKeyboardInput(&in->KeyboardController.MoveForward, isDown);
            } break;

            case VK_DOWN_ARROW:
            {
                MXBXProcessKeyboardInput(&in->KeyboardController.MoveBackward, isDown);
            } break;

            case VK_LEFT_ARROW:
            {
                MXBXProcessKeyboardInput(&in->KeyboardController.StrafeLeft, isDown);
            } break;

            case VK_RIGHT_ARROW:
            {
                MXBXProcessKeyboardInput(&in->KeyboardController.StrafeRight, isDown);
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
MXBXBlitbackbuffer()
{
    // TODO(Jovan): //(renderer->BackbufferW * renderer->BackbufferH * renderer->BackbufferStride)); fix?
	asm("push r1\npush r2\n push r3\nmov.w r1, 1024\nmov.w r2, %0\nmov.w r3, %1\nblit\npop r3\npop r2\npop r1\n"
		: /* No output */
		: "r" (R_Backbuffer), "i" (BACKBUFFER_W * BACKBUFFER_H * 2));
}

int
main()
{
    init_stdio();
    video_mode(1);
    cls();
    i8 str[256];

    LUTInit();

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
    renderer.Backbuffer = R_Backbuffer;

    // NOTE(Jovan): Dithering pattern
	// for(u32 i = 0; i < BACKBUFFER_H * BACKBUFFER_W; ++i)
    //     if((i / BACKBUFFER_W) % 2 == 0)
	// 	    __emptybackbuffer[i] = 0x0707;
    //     else
	// 	    __emptybackbuffer[i] = 0x7070;

    // TODO(Jovan): Separate game_state from platform layer using memory arenas
    game_state state = {0};

    i32 startCounter = get_millis();
    // GAME LOOP
    while(1)
    {
        mxbx_keyboard *oldKeyboardController = &oldInput->KeyboardController;
        mxbx_keyboard *newKeyboardController = &newInput->KeyboardController;
        *newKeyboardController = (mxbx_keyboard){0};

        for(i32 buttonIndex = 0;
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
        UpdateAndRender(&state, newInput, &renderer);
        MXBXBlitbackbuffer();

        i32 workCounter = get_millis() - startCounter;

        // NOTE(Jovan): Try retaining constant FPS
        if(workCounter < TARGET_MS_PER_FRAME)
        {
            delay(TARGET_MS_PER_FRAME - workCounter);
        }
        i32 endCounter = get_millis();
        // sprintf(str, "MS Per frame: %d", endCounter - startCounter);
        // draw(10, 10, RED, str);
        startCounter = endCounter;

        mxbx_input *tmp = newInput;
        newInput = oldInput;
        oldInput = tmp;
    }

    return 0;
}
