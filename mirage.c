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
		state->Player.DX = FPMul(state->Player.Cos, 0x20000);
		state->Player.DY = FPMul(state->Player.Sin, 0x20000);
    }

    if(input->KeyboardController.StrafeRight.EndedDown)
    {
		state->Player.Angle += BAM1 * 5;
		state->Player.Cos = LUTFPCos[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.Sin = LUTFPSin[state->Player.Angle >> BAM_LUT_SHIFT];
		state->Player.DX = FPMul(state->Player.Cos, 0x20000);
		state->Player.DY = FPMul(state->Player.Sin, 0x20000);
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
			DrawRect(renderer, i * MapTileSize + 1, j * MapTileSize + 1, (MapTileSize - 1), (MapTileSize - 1), color);
		}
	}

    
	i32 px = state->Player.X >> FP_SCALE;
	i32 py = state->Player.Y >> FP_SCALE;
	i32 mx = px;
	i32 my = py;
	DrawRect(renderer, mx, my, 3, 3, 0x6666);
	i32 cos = __fparsh(FPMul(state->Player.Cos, 0x50000), FP_SCALE);
	i32 sin = __fparsh(FPMul(state->Player.Sin, 0x50000), FP_SCALE);
	i32 lx = mx + cos;
	i32 ly = my + sin;
	DrawLine(renderer, mx + 1, my + 1, lx, ly, 0x6666);

	q16 planeX = 0;
	q16 planeY = 0xA8F5; // 0.66

	// NOTE(Jovan): Raycast
	// TODO(Jovan): Fast invsqrt
	q16 rayStartX = state->Player.X + 0x10000;
	q16 rayStartY = state->Player.Y + 0x10000;
	// int rayAngle = Player.Angle - FPMul(0x1E0000, DegInRad); // 30 * DegInRad
	i32 rayAngle = state->Player.Angle - BAM30;
	u8 str[256];
	for(u16 ray = 0;
		ray < 30;
		++ray)
	{
		// int rayDirX = FPCos(rayAngle);
		// int rayDirY = FPSin(rayAngle);
		q16 rayDirX = LUTFPCos[rayAngle >> BAM_LUT_SHIFT];
		q16 rayDirY = LUTFPSin[rayAngle >> BAM_LUT_SHIFT];
		// NOTE(Jovan): lodev.org optimization
		q16 rayStepSizeX = !(rayDirY & rayDirY) ? 0 : (!(rayDirX & rayDirX) ? 0x10000 : ABS(FPDiv(0x10000, rayDirX)));
		q16 rayStepSizeY = !(rayDirX & rayDirX) ? 0 : (!(rayDirY & rayDirY) ? 0x10000 : ABS(FPDiv(0x10000, rayDirY)));

		// NOTE(Jovan): Whole part only
		q16 fpMapCheckX = rayStartX & 0xFFFF0000;
		q16 fpMapCheckY = rayStartY & 0xFFFF0000;
		q16 rayLenX = 0;
		q16 rayLenY = 0;
		i32 rayStepX;
		i32 rayStepY;
		// NOTE(Jovan): Left
		if(rayDirX & FP_NEG_MASK)
		{
			rayStepX = -1;
			rayLenX = FPMul((rayStartX - fpMapCheckX), rayStepSizeX);
		}
		else
		{
			rayStepX = 1;
			rayLenX = FPMul((fpMapCheckX + 0x10000 - rayStartX), rayStepSizeX);
		}

		// NOTE(Jovan): Up
		if(rayDirY & FP_NEG_MASK)
		{
			rayStepY = -1;
			rayLenY = FPMul((rayStartY - fpMapCheckY), rayStepSizeY);
		}
		else
		{
			rayStepY = 1;
			rayLenY = FPMul((fpMapCheckY + 0x10000 - rayStartY), rayStepSizeY);
		}

		i32 tileFound = 0;
		i32 dof = 0;
		q16 distance = 0;
		i32 side = 0;
		i32 mapCheckX = fpMapCheckX >> (FP_SCALE + 4);
		i32 mapCheckY = fpMapCheckY >> (FP_SCALE + 4);
		i16 tile;
		while(!tileFound && dof++ < 8)
		{
			if(rayLenX < rayLenY)
			{
				mapCheckX += rayStepX;
				rayLenX += rayStepSizeX;
				distance = rayLenX;
				side = 0;
			}
			else
			{
				mapCheckY += rayStepY;
				rayLenY += rayStepSizeY;
				distance = rayLenY;
				side = 1;
			}

			i32 mapIndex = mapCheckY * MapX + mapCheckX;
			if(mapCheckX >= 0 && mapCheckX < MapX && mapCheckY >= 0 && mapCheckY < MapY)
			{
				tile = Map[mapIndex];
				if(tile > 0)
				{
					++tileFound;
					DrawRect(renderer, mapCheckX * MapTileSize + 1, mapCheckY * MapTileSize + 1, (MapTileSize - 1), (MapTileSize - 1), 0x1111);
				}
			}
		}

		// return;
		if(tileFound)
		{
			// distance = FPMul(distance, 0xF0000); // * 15
			// int intersectionX = rayStartX + FPMul(rayDirX, distance);
			// int intersectionY = rayStartY + FPMul(rayDirY, distance);
			// __drawline(px, py, intersectionX >> FP_SCALE, intersectionY >> FP_SCALE, 0x1111);
			q16 perpWallDist;
			fpMapCheckX = (mapCheckX * 1 << FP_SCALE) << 4;
			fpMapCheckY = (mapCheckY * 1 << FP_SCALE) << 4;

			// TODO(Jovan): Fish-eye effect still ocurring???
			if(side == 0)
			{
				//(mapX - posX + (1 - stepX) / 2) / rayDirX;
				perpWallDist = FPDiv(fpMapCheckX - rayStartX + FPDiv(0x10000 - (rayStepX * (1 << FP_SCALE)), 0x20000), rayDirX);
			}
			else
			{
				perpWallDist = FPDiv(fpMapCheckY - rayStartY + FPDiv(0x10000 - (rayStepY * (1 << FP_SCALE)), 0x20000), rayDirY);
			}
			// 0x3C0000 = 60
			i32 h = 240;
			q16 fph = 0xF00000;
			i32 lineHeight = FPDiv79(fph, perpWallDist) >> FP_SCALE;
			// int drawStart = -lineHeight / 2 + h / 2;
			// if(drawStart < 0)drawStart = 0;
			// int drawEnd = lineHeight / 2 + h / 2;
			// if(drawEnd >= h) drawEnd = h - 1;
			if(lineHeight > 0 && lineHeight < 240)
			{
				DrawRect(renderer, 200 + ray*2, (h - lineHeight * 10) / 2, 2, lineHeight * 10, tile == 2 ? 0x4444 : 0x3333);
			}
		}
		// rayAngle += 0x8EF; // 2 deg in rad
		rayAngle += BAM2;
	}
}