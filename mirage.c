#include "mirage.h"
#include "intrinsics.h"
#include "renderer.c"
#include "tables.c"

q16 SpawnX = 0x20000; //0x170000;
q16 SpawnY = 0x20000; //0x170000;
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
        state->Player.X += state->Player.DirX;
        state->Player.Y += state->Player.DirY;
    }

    if(input->KeyboardController.MoveBackward.EndedDown)
    {
		i32 newX = state->Player.X - state->Player.DirX;
		i32 newY = state->Player.Y - state->Player.DirY;
		state->Player.X = (newX & FP_NEG_MASK) ? 0 : newX;
		state->Player.Y = (newY & FP_NEG_MASK) ? 0 : newY;
    }

    if(input->KeyboardController.StrafeLeft.EndedDown)
    {
        i32 strafeAngle = - (BAM1 * 5);
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
        i32 strafeAngle = BAM1 * 5;
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

// void
// RenderMinimap(game_state* state, mxbx_renderer *renderer, i32 x, i32 y)
// {
// 	for(i32 i = 0;
// 		i < MapX;
// 		++i)
// 	{
// 		for(i32 j = 0;
// 			j < MapY;
// 			++j)
// 		{
// 			i16 color = 0;
// 			i16 tile = Map[j * MapX + i];
// 			if(tile == 1) color = 0x7777;
// 			else if(tile == 2) color = 0x4444;
// 			DrawRect(renderer, x + i * MapTileSize + 1, y + j * MapTileSize + 1, (MapTileSize - 1), (MapTileSize - 1), color);
// 		}
// 	}
    
// 	i32 px = state->Player.X >> FP_SCALE;
// 	i32 py = state->Player.Y >> FP_SCALE;
// 	i32 mx = px >> 2;
// 	i32 my = py >> 2;
// 	DrawRect(renderer, mx, my, 3, 3, 0x6666);
// 	i32 cos = __fparsh(FPMul(state->Player.DirX, 0x50000), FP_SCALE);
// 	i32 sin = __fparsh(FPMul(state->Player.DirY, 0x50000), FP_SCALE);
// 	i32 lx = mx + cos;
// 	i32 ly = my + sin;
// 	DrawLine(renderer, mx + 1, my + 1, lx, ly, 0x6666);
// }


float ToFloat(int x) {
    return x / (float) (1 << FP_SCALE);
}

int ToFixed(float x) {
    return x * (float)(1 << FP_SCALE);
}

void
Raycast(game_state *state, mxbx_input *input, mxbx_renderer *renderer)
{
	   // NOTE(Jovan): Raycast
    int w = 80;
    int h = 60;
    int planeX = state->Player.PlaneX;
    int planeY = state->Player.PlaneY;
    int posX = state->Player.X;
    int posY = state->Player.Y;
    int dirX = state->Player.DirX;
    int dirY = state->Player.DirY;
    for(int x = 0; x < w; x++)
    {
      //calculate ray position and direction
      int cameraX = FPDiv511(x << (FP_SCALE + 1), w << FP_SCALE) - 0x10000;
      // int cameraX = ToFixed(2 * x / (float)w - 1); //x-coordinate in camera space
      int rayDirX = dirX + FPMul(planeX, cameraX);
      int rayDirY = dirY + FPMul(planeY, cameraX);
      //which box of the map we're in
      int mapX = posX & 0xFFFF0000;
      int mapY = posY & 0xFFFF0000;

      //length of ray from current position to next x or y-side
      int sideDistX;
      int sideDistY;

       //length of ray from one x or y-side to next x or y-side
      // float deltaDistX = ABS((1.0f / ToFloat(rayDirX)));
      int deltaDistY = ABS(FPDiv(0x10000, rayDirY));
      int deltaDistX = ABS(FPDiv(0x10000, rayDirX));
      // printf("deltaDistX: %f, fp: %f, 0x%X\n", deltaDistX, ToFloat(fDeltaDistX), fDeltaDistX);
      int perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?
      //calculate step and initial sideDist
      if(rayDirX & 0xF0000000)
      {
        // stepX = -1;
        stepX = 0xFFFF0000;
        // sideDistX = (posX - mapX) * deltaDistX;
        int fix = posX - mapX;
        sideDistX = FPMul(fix, deltaDistX);
      }
      else
      {
        // stepX = 1;
        stepX = 0x10000;
        // sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        int fix = mapX + 0x10000 - posX;
        sideDistX = FPMul(fix, deltaDistX);
      }
      if(rayDirY & 0xF0000000)
      {
        // stepY = -1;
        stepY = 0xFFFF0000;
        // sideDistY = (posY - mapY) * deltaDistY;
        int fix = posY - mapY;
        sideDistY = FPMul(fix, deltaDistY);
      }
      else
      {
        // stepY = 1;
        stepY = 0x10000;
        // sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        int fix = mapY + 0x10000 - posY;
        sideDistY = FPMul(fix, deltaDistY);
      }
      //perform DDA
      while (hit == 0)
      {
        //jump to next map square, OR in x-direction, OR in y-direction
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
        //Check if ray has hit a wall
        if(Map[mapY >> FP_SCALE][mapX >> FP_SCALE] > 0) hit = 1;
      }
      //Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
      if(side == 0)
      {
        int res = mapX - posX + FPDiv((0x10000 - stepX), 0x20000);
        perpWallDist = FPDiv(res, rayDirX);
      }
      else
      {
        int res = mapY - posY + FPDiv((0x10000 - stepY), 0x20000);
        perpWallDist = FPDiv(res, rayDirY);
      }

      //Calculate height of line to draw on screen
      int lineHeight = ToFloat(FPDiv79(ToFixed(h), perpWallDist));

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2;
      if(drawStart < 0)drawStart = 0;
      int drawEnd = lineHeight / 2 + h / 2;
      if(drawEnd >= h)drawEnd = h - 1;

      //draw the pixels of the stripe as a vertical line
      DrawLine(renderer, x, drawStart, x, drawEnd, 0x1111);
    }
}

void
UpdateAndRender(game_state *state, mxbx_input *input, mxbx_renderer *renderer)
{
    if(!state->IsInitialized)
    {
        state->Player.X = SpawnX;
        state->Player.Y = SpawnY;
        state->Player.DirX = 0xFFFF0000;
        state->Player.DirY = 0;
        state->Player.PlaneX = 0;
        state->Player.PlaneY = 0xA8F5; // 0.66;

        state->IsInitialized = 1;
    }

    HandleInput(state, input);
	ClearBackbuffer();
    // NOTE(Jovan): 0, 0 while debugging
    // RenderMinimap(state, renderer, 0, 0);

	// NOTE(Jovan): Raycast
  Raycast(state, input, renderer);
	// int w = 80;
	// int h = 60;
	// q16 planeX = state->Player.PlaneX;
	// q16 planeY = state->Player.PlaneY;
	// q16 posX = state->Player.X;
	// q16 posY = state->Player.Y;
	// q16 dirX = state->Player.DirX;
	// q16 dirY = state->Player.DirY;
	// u8 str[256];
	// for(u16 x = 0;
	// 	x < w;
	// 	++x)
	// {
    //     q16 cameraX = FPDiv(x << (FP_SCALE + 1), w << FP_SCALE) - 0x10000;
    //     q16 rayDirX = dirX + FPMul(planeX, cameraX);
    //     q16 rayDirY = dirY + FPMul(planeY, cameraX);
	// 	i32 mapX = posX >> FP_SCALE;
	// 	i32 mapY = posY >> FP_SCALE;
	// 	q16 sideDistX;
	// 	q16 sideDistY;
		
	// 	// NOTE(Jovan): lodev.org optimization
	// 	q16 deltaDistX = (rayDirY == 0) ? 0 : ((rayDirX == 0) ? 0x10000 : ABS(FPDiv(0x10000, rayDirX)));
	// 	q16 deltaDistY = (rayDirX == 0) ? 0 : ((rayDirY == 0) ? 0x10000 : ABS(FPDiv(0x10000, rayDirY)));
	// 	q16 perpWallDist;

	// 	i32 stepX;
	// 	i32 stepY;
	// 	i32 hit = 0;
	// 	i32 side;
	// 	// NOTE(Jovan): Left
	// 	if(rayDirX & FP_NEG_MASK)
	// 	{
	// 		stepX = -1;
	// 		sideDistX = FPMul((posX - (mapX << FP_SCALE)), deltaDistX);
	// 	}
	// 	else
	// 	{
	// 		stepX = 1;
	// 		sideDistX = FPMul(((mapX << FP_SCALE) + 0x10000 - posX), deltaDistX);
	// 	}

	// 	// NOTE(Jovan): Up
	// 	if(rayDirY & FP_NEG_MASK)
	// 	{
	// 		stepY = -1;
	// 		sideDistY = FPMul((posY - (mapY << FP_SCALE)), deltaDistY);
	// 	}
	// 	else
	// 	{
	// 		stepY = 1;
	// 		sideDistY = FPMul(((mapY << FP_SCALE) + 0x10000 - posY), deltaDistY);
	// 	}

	// 	int dof = 0;
	// 	while(hit == 0)
	// 	{
	// 		if(sideDistX < sideDistY)
	// 		{
	// 			sideDistX += deltaDistX;
	// 			mapX += stepX;
	// 			side = 0;
	// 		}
	// 		else
	// 		{
	// 			sideDistY += deltaDistY;
	// 			mapY += stepY;
	// 			side = 1;
	// 		}

	// 		if(Map[mapY][mapX] > 0)
	// 		{
	// 			hit = 1;
	// 		}
	// 	}

	// 	if(side == 0)
	// 	{
	// 		//(mapX - posX + (1 - stepX) / 2) / rayDirX;
	// 		perpWallDist = FPDiv((mapX << FP_SCALE) - posX + FPDiv(0x10000 - (stepX << FP_SCALE), 0x20000), rayDirX);
	// 	}
	// 	else
	// 	{
	// 		perpWallDist = FPDiv((mapY << FP_SCALE) - posY + FPDiv(0x10000 - (stepY << FP_SCALE), 0x20000), rayDirY);
	// 	}
	// 	q16 fph = h << FP_SCALE;
	// 	i32 lineHeight = FPDiv79(fph, perpWallDist) >> FP_SCALE;

	// 	if (lineHeight >= h)
	// 	{
	// 		lineHeight = h - 1;
	// 	}
	// 	i32 lineStart = -lineHeight / 2 + h / 2;
	// 	if(lineStart < 0)
	// 	{
	// 		lineStart = 0;
	// 	}
	// 	i32 lineEnd = lineHeight / 2 + h / 2;
	// 	if(lineEnd >= h)
	// 	{
	// 		lineEnd = h - 1;
	// 	}
	// 	DrawLine(renderer, x, lineStart, x, lineEnd, 0x3333);
	// }
}
