#include "mirage.h"

void
ProcessKeyboardInput(button_state *newState, char isDown)
{
    newState->EndedDown = isDown;
    ++newState->HalfTransitionCount;
}

char
HandleInput(input *in)
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
                ProcessKeyboardInput(&in->KeyboardController.StrafeLeft, isDown);
            } break;

            case VK_ESC:
            {
                shouldQuit = 1;
            } break;
        }
    }
    return shouldQuit;
}

int
main()
{
    init_stdio();
    video_mode(1);
    cls();
    char str[256];
    int startCounter = get_millis();

    input inputs[2];
    input *newInput = &inputs[0];
    input *oldInput = &inputs[1];
    *newInput = (input){0};
    *oldInput = (input){0};
    // GAME LOOP
    while(1)
    {
        cls();
        keyboard *oldKeyboardController = &oldInput->KeyboardController;
        keyboard *newKeyboardController = &newInput->KeyboardController;
        *newKeyboardController = (keyboard){0};

        for(int buttonIndex = 0;
            buttonIndex < ArrayCount(newKeyboardController->Buttons);
            ++buttonIndex)
        {
            newKeyboardController->Buttons[buttonIndex].EndedDown =
                oldKeyboardController->Buttons[buttonIndex].EndedDown;
        }

        if(HandleInput(newInput))
        {
            cls();
            break;
        }

        if(newInput->KeyboardController.StrafeLeft.EndedDown)
        {
            sprintf(str,"LEFT");
            draw(80, 80, YELLOW, str);
        }

        int workCounter = get_millis() - startCounter;
        if(workCounter < TARGET_MS_PER_FRAME)
        {
            delay(TARGET_MS_PER_FRAME - workCounter);
        }
        int endCounter = get_millis();
        sprintf(str, "MS Per frame: %d", endCounter - startCounter);
        draw(10, 10, RED, str);
        startCounter = endCounter;

        input *tmp = newInput;
        newInput = oldInput;
        oldInput = tmp;
    }

    return 0;
}