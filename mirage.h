#include <stdio.h>
#include <string.h>
#include <sprintf.h>
#include <consts.h>
#include <graphics.h>

#define TARGET_MS_PER_FRAME 34

#define BLUE   0x1
#define GREEN  0x2
#define AQUA   0x3
#define RED    0x4
#define PURPLE 0x5
#define YELLOW 0x6
#define WHITE  0x7

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

typedef struct button_state_st
{
    int HalfTransitionCount;
    char EndedDown;
} button_state;

typedef struct keyboard_st
{
    union
    {
        button_state Buttons[4];
        struct
        {
            button_state MoveForward;
            button_state MoveBackward;
            button_state StrafeLeft;
            button_state StrafeRight;
        };
    };
} keyboard;

typedef struct input_st
{
    keyboard KeyboardController;
} input;