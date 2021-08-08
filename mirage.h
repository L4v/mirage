#ifndef MIRAGE_H

#include "fixed.c"

#define TARGET_MS_PER_FRAME 34 // ~30fps

#define BLUE   0x1
#define GREEN  0x2
#define AQUA   0x3
#define RED    0x4
#define PURPLE 0x5
#define YELLOW 0x6
#define WHITE  0x7

typedef struct button_state_st
{
    i32 HalfTransitionCount;
    i8 EndedDown;
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

typedef struct st_player
{
	q16 X;
	q16 Y;
	q16 DX;
	q16 DY;
	i32 Angle;
	q16 Sin;
	q16 Cos;
} player;

// TODO(Jovan): Memory arenas?
typedef struct st_game_state
{
    player Player;
    i8 IsInitialized;
} game_state;

#define MIRAGE_H
#endif