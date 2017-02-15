//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "loadSDL.h"
#include <time.h>

const int WALKING_ANIMATION_FRAMES = 4;

int main(int argc, char* args[])
{
	loadSDL sdl;
	//Start up SDL and create window
	if (!sdl.init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!sdl.loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Current animation frame
			int frame = 0;

			//Random background
			srand(time(0));
			int randomBackground = rand() % 4;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor(sdl.gRenderer, 255, 255, 255, 0xFF);
				SDL_RenderClear(sdl.gRenderer);

				//render the background
				SDL_Rect* backgroundClip = &sdl.backgroundSprite[randomBackground];
				sdl.render(0,0,sdl.gTextureBackground,backgroundClip,true);

				// add belnd mode to render
				SDL_SetRenderDrawBlendMode(sdl.gRenderer, SDL_BLENDMODE_BLEND);
				
				//Render red filled quad
				SDL_Rect fillRect = { 50, 50, 390, 515 };
				SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 80);
				SDL_RenderFillRect(sdl.gRenderer, &fillRect);
				
				for (int i = 0; i < 10; i++)
				{
					//Render green outlined quad
					SDL_Rect outlineRect = { 50 + i , 50 + i, 390 - (i * 2), 515 - (i * 2) };
					SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 255, 255);
					SDL_RenderDrawRect(sdl.gRenderer, &outlineRect);
				}
								
				//print red puyos
				for (int y = 0; y < 16; y++)
				{
					for (int x = 0; x < 11; x++)
					{
						//Render current frame			
						SDL_Rect* puyoClip = &sdl.redPuyoSprite[frame];
						sdl.render(60 + (33 * x), 60 + (31 * y), sdl.gTexturePuyo, puyoClip, false);
					}
				}
				
				//Update screen
				SDL_RenderPresent(sdl.gRenderer);

				SDL_Delay(150);

				//Go to next frame
				++frame;

				//Cycle animation
				if (frame / 4 >= WALKING_ANIMATION_FRAMES)
				{
					frame = 0;
				}
			}
		}
	}

	//Free resources and close SDL
	sdl.close();

	return 0;
}
