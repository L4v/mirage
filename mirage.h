#ifndef MIRAGE_H

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
} mxbx_button_state;

typedef struct mxbx_keyboard_st
{
    union
    {
        mxbx_button_state Buttons[4];
        struct
        {
            mxbx_button_state MoveForward;
            mxbx_button_state MoveBackward;
            mxbx_button_state StrafeLeft;
            mxbx_button_state StrafeRight;
        };
    };
} mxbx_keyboard;

typedef struct mxbx_input_st
{
    mxbx_keyboard KeyboardController;
} mxbx_input;

typedef struct mxbx_renderer_st
{
    short *Backbuffer;
    int BackbufferW;
    int BackbufferH;
    int BackbufferStride;
} mxbx_renderer;

#define MIRAGE_H
#endif