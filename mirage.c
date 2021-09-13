#include "mirage.h"
#include "intrinsics.h"
#include "renderer.c"
#include "tables.c"

q16 SpawnX = 0x20000;
q16 SpawnY = 0x20000;
i32 MapX = 16, MapY = 16, MapS = 256;
i32 MapTileSize = 4;
i32 Map[][16] = 
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

i32 TexWidth = 16;
i16 Texture[] = 
{
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x4440, 0x4440, 0x4440, 0x4440,
    0x0000, 0x0000, 0x0000, 0x0000,
};

void
HandleInput(game_state *state, mxbx_input *input)
{
    if(input->KeyboardController.MoveForward.EndedDown)
    {
        q16 newX = state->Player.X + state->Player.DirX;
        q16 newY = state->Player.Y + state->Player.DirY;
        i32 mapX = newX >> FP_SCALE;
        i32 mapY = newY >> FP_SCALE;
        if(mapX  >= 0 && mapX < MapX
            && mapY >= 0 && mapY < MapY
            && Map[mapY][mapX] == 0)
        {
            state->Player.X = (newX & FP_NEG_MASK) ? 0 : newX;
            state->Player.Y = (newY & FP_NEG_MASK) ? 0 : newY;
        }
        
    }

    if(input->KeyboardController.MoveBackward.EndedDown)
    {
        q16 newX = state->Player.X - state->Player.DirX;
        q16 newY = state->Player.Y - state->Player.DirY;
        i32 mapX = newX >> FP_SCALE;
        i32 mapY = newY >> FP_SCALE;
        if(mapX  >= 0 && mapX < MapX
            && mapY >= 0 && mapY < MapY
            && Map[mapY][mapX] == 0)
        {
            state->Player.X = (newX & FP_NEG_MASK) ? 0 : newX;
            state->Player.Y = (newY & FP_NEG_MASK) ? 0 : newY;
        }
    }

    if(input->KeyboardController.StrafeLeft.EndedDown)
    {
        i32 strafeAngle = -(BAM1 * 5);
        q16 oldDirX = state->Player.DirX;
        q16 cos = LUTFPCos[strafeAngle >> BAM_LUT_SHIFT];
        q16 sin = LUTFPSin[strafeAngle >> BAM_LUT_SHIFT];
        state->Player.DirX = FPMul(oldDirX, cos) - FPMul(state->Player.DirY, sin);
        state->Player.DirY = FPMul(state->Player.DirY, cos) + FPMul(oldDirX, sin);
        
        q16 oldPlaneX = state->Player.PlaneX;
        state->Player.PlaneX = FPMul(oldPlaneX, cos) - FPMul(state->Player.PlaneY, sin);
        state->Player.PlaneY = FPMul(state->Player.PlaneY, cos) + FPMul(oldPlaneX, sin);
    }

    if(input->KeyboardController.StrafeRight.EndedDown)
    {
        i32 strafeAngle = (BAM1 * 5);
        q16 oldDirX = state->Player.DirX;
        q16 cos = LUTFPCos[strafeAngle >> BAM_LUT_SHIFT];
        q16 sin = LUTFPSin[strafeAngle >> BAM_LUT_SHIFT];
        state->Player.DirX = FPMul(oldDirX, cos) - FPMul(state->Player.DirY, sin);
        state->Player.DirY = FPMul(state->Player.DirY, cos) + FPMul(oldDirX, sin);

        q16 oldPlaneX = state->Player.PlaneX;
        state->Player.PlaneX = FPMul(oldPlaneX, cos) - FPMul(state->Player.PlaneY, sin);
        state->Player.PlaneY = FPMul(state->Player.PlaneY, cos) + FPMul(oldPlaneX, sin);
    }

}

void
RenderMinimap(game_state* state, mxbx_renderer *renderer, i32 x, i32 y)
{
	for(i32 i = 0;
		i < MapX;
		++i)
	{
		for(i32 j = 0;
			j < MapY;
			++j)
		{
			i16 color = 0;
			i16 tile = Map[j][i];
			if(tile == 1) color = 0x7777;
			else if(tile == 2) color = 0x4444;
			DrawRect(renderer, x + i * MapTileSize + 1, y + j * MapTileSize + 1, (MapTileSize - 1), (MapTileSize - 1), color);
		}
	}
    
	i32 px = state->Player.X >> FP_SCALE;
	i32 py = state->Player.Y >> FP_SCALE;
	i32 mx = px << 2;
	i32 my = py << 2;
	DrawRect(renderer, mx, my, 3, 3, 0x6666);
	i32 cos = __fparsh(FPMul(state->Player.DirX, 0x50000), FP_SCALE);
	i32 sin = __fparsh(FPMul(state->Player.DirY, 0x50000), FP_SCALE);
	i32 lx = mx + cos;
	i32 ly = my + sin;
	DrawLine(renderer, mx + 1, my + 1, lx, ly, 0x6666);
}

float ToFloat(q16 x) {
    return x / (float) (1 << FP_SCALE);
}

q16 ToFixed(float x) {
    return x * (float)(1 << FP_SCALE);
}

void
Raycast(game_state *state, mxbx_input *input, mxbx_renderer *renderer, i32 xOffset, i32 yOffset)
{
    i32 w = 160;
    i32 h = 120;
    q16 planeX = state->Player.PlaneX;
    q16 planeY = state->Player.PlaneY;
    q16 posX = state->Player.X;
    q16 posY = state->Player.Y;
    q16 dirX = state->Player.DirX;
    q16 dirY = state->Player.DirY;
    // NOTE(Jovan): "Optimization" with no significant improvement`
    // q16 cameraX2 = 0;
    // q16 cameraXInc = ToFixed(1.0f / (float)w); //FPDiv511(0x10000, w << FP_SCALE);
    for(i32 x = 0; x < w; x++)
    {
        // q16 cameraX = FPMul(x << (FP_SCALE + 1), cameraXInc) - 0x10000;
        // NOTE(Jovan): Calculating ray direction via camera plane
        q16 cameraX = FPDiv511(x << (FP_SCALE + 1), w << FP_SCALE) - 0x10000;
        // int cameraX = ToFixed(2 * x / (float)w - 1); //x-coordinate in camera space
        q16 rayDirX = dirX + FPMul(planeX, cameraX);
        q16 rayDirY = dirY + FPMul(planeY, cameraX);

        // NOTE(Jovan): Get whole part of player coordinate
        q16 mapX = posX & 0xFFFF0000;
        q16 mapY = posY & 0xFFFF0000;

        // NOTE(Jovan): Offset from current in-tile position to next X and Y edge
        q16 sideDistX;
        q16 sideDistY;

        // NOTE(Jovan): Distance from one X/Y edge to another
        // NOTE(Jovan): Lodev.org optimization
        q16 deltaDistY = ABS(FPDiv(0x10000, rayDirY));
        q16 deltaDistX = ABS(FPDiv(0x10000, rayDirX));

        //what direction to step in x or y-direction (either +1 or -1)
        q16 stepX;
        q16 stepY;

        // NOTE(Jovan): hit and NS/EW collision flags
        u32 hit = 0;
        u32 side;

        // NOTE(Jovan): Setting initial distance values for iterations
        if(rayDirX & 0xF0000000)
        {
            stepX = 0xFFFF0000;
            q16 fix = posX - mapX;
            sideDistX = FPMul(fix, deltaDistX);
        }
        else
        {
            stepX = 0x10000;
            q16 fix = mapX + 0x10000 - posX;
            sideDistX = FPMul(fix, deltaDistX);
        }
        if(rayDirY & 0xF0000000)
        {
            stepY = 0xFFFF0000;
            q16 fix = posY - mapY;
            sideDistY = FPMul(fix, deltaDistY);
        }
        else
        {
            stepY = 0x10000;
            q16 fix = mapY + 0x10000 - posY;
            sideDistY = FPMul(fix, deltaDistY);
        }

        // NOTE(Jovan): DDA
        while (hit == 0)
        {
            if(sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            
            if(Map[mapY >> FP_SCALE][mapX >> FP_SCALE] > 0) 
            {
                hit = 1;
            };
        }

        // NOTE(Jovan): lodev.org trick, calculate perpendicular distance,
        // distance from camera plane to wall as to avoid fish-eye effect
        q16 perpWallDist;
        if(side == 0)
        {
            q16 res = mapX - posX + FPDiv((0x10000 - stepX), 0x20000);
            perpWallDist = FPDiv(res, rayDirX);
        }
        else
        {
            q16 res = mapY - posY + FPDiv((0x10000 - stepY), 0x20000);
            perpWallDist = FPDiv(res, rayDirY);
        }

        // NOTE(Jovan): Draw vertical scan lines
        // Doesn't work with i32????
        int lineHeight = (FPDiv79(ToFixed(h), perpWallDist)) >> FP_SCALE;

        i32 drawStart = -lineHeight / 2 + h / 2;
        if(drawStart < 0)
        {
            drawStart = 0;
        }
        i32 drawEnd = lineHeight / 2 + h / 2;
        if(drawEnd >= h)
        {
            drawEnd = h - 1;
        }

        DrawLine(renderer, x + xOffset, drawStart + yOffset, x + xOffset, drawEnd + yOffset, 0x1111);
    }
}

void
UpdateAndRender(game_state *state, mxbx_input *input, mxbx_renderer *renderer)
{
    if(!state->IsInitialized)
    {
        state->Player.X = SpawnX;
        state->Player.Y = SpawnY;
        state->Player.DirX = 0x10000;
        state->Player.DirY = 0;
        state->Player.PlaneX = 0;
        state->Player.PlaneY = 0xA8F5; // 0.66;

        state->IsInitialized = 1;
    }

    HandleInput(state, input);
    ClearBackbuffer();
    // NOTE(Jovan): 0, 0 while debugging
    RenderMinimap(state, renderer, 0, 0);

    Raycast(state, input, renderer, 60, 80);
}
