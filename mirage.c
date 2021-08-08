#include "mirage.h"
#include "intrinsics.h"
#include "renderer.c"
#include "tables.c"

i32 MapX = 12, MapY = 12, MapS = 144;
i32 MapTileSize = 16;
i32 Map[] = 
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

void
HandleInput(game_state *state, mxbx_input *input)
{
    if(input->KeyboardController.MoveForward.EndedDown)
    {
        state->Player.X += state->Player.DX;
        state->Player.Y += state->Player.DY;
    }

    if(input->KeyboardController.MoveBackward.EndedDown)
    {
		i32 newX = state->Player.X - state->Player.DX;
		i32 newY = state->Player.Y - state->Player.DY;
		state->Player.X = (newX & FP_NEG_MASK) ? 0 : newX;
		state->Player.Y = (newY & FP_NEG_MASK) ? 0 : newY;
    }

    if(input->KeyboardController.StrafeLeft.EndedDown)
    {
		state->Player.Angle -= BAM1 * 5;
		state->Player.Cos = LUTFPCos[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.Sin = LUTFPSin[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.DX = FPMul(state->Player.Cos, 0x50000);
		state->Player.DY = FPMul(state->Player.Sin, 0x50000);
    }

    if(input->KeyboardController.StrafeRight.EndedDown)
    {
		state->Player.Angle += BAM1 * 5;
		state->Player.Cos = LUTFPCos[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.Sin = LUTFPSin[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.DX = FPMul(state->Player.Cos, 0x50000);
		state->Player.DY = FPMul(state->Player.Sin, 0x50000);
    }

}

void
UpdateAndRender(game_state *state, mxbx_input *input, mxbx_renderer *renderer)
{
    if(!state->IsInitialized)
    {
        state->Player.X = 0xA00000;
        state->Player.Y = 0x100000;
        state->Player.DX = 0;
        state->Player.DY = 0;
        state->Player.Angle = BAM90; //HalfPI;
        state->Player.Cos = LUTFPCos[state->Player.Angle >> BAM_LUT_SHIFT];//FPCos(Player.Angle);
        state->Player.Sin = LUTFPSin[state->Player.Angle >> BAM_LUT_SHIFT];//FPSin(Player.Angle);

        state->IsInitialized = 1;
    }

    HandleInput(state, input);
	ClearBackbuffer();

	for(i32 i = 0;
		i < MapX;
		++i)
	{
		for(i32 j = 0;
			j < MapY;
			++j)
		{
			i16 color = 0;
			i16 tile = Map[j * MapX + i];
			if(tile == 1) color = 0x7777;
			else if(tile == 2) color = 0x4444;
			DrawRect(renderer, i * 16 + 1, j * 16 + 1, 15, 15, color);
		}
	}

    
	i32 px = state->Player.X >> FP_SCALE;
	i32 py = state->Player.Y >> FP_SCALE;
	DrawRect(renderer, px, py, 3, 3, 0x6666);
	i32 cos = __fparsh(FPMul(state->Player.Cos, 0xA0000), FP_SCALE);
	i32 sin = __fparsh(FPMul(state->Player.Sin, 0xA0000), FP_SCALE);
	i32 lx = px + cos;
	i32 ly = py + sin;
	DrawLine(renderer, px + 1, py + 1, lx, ly, 0x6666);
}