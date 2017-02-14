/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "loadSDL.h"

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

				// add belnd mode to render
				SDL_SetRenderDrawBlendMode(sdl.gRenderer, SDL_BLENDMODE_BLEND);
				
				//Render red filled quad
				SDL_Rect fillRect = { sdl.SCREEN_WIDTH / 4, sdl.SCREEN_HEIGHT / 4, sdl.SCREEN_WIDTH / 2, sdl.SCREEN_HEIGHT / 2 };
				SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 80);
				SDL_RenderFillRect(sdl.gRenderer, &fillRect);
				
				for (int i = 0; i < 10; i++)
				{
					//Render green outlined quad
					SDL_Rect outlineRect = { (sdl.SCREEN_WIDTH / 4) + i , (sdl.SCREEN_HEIGHT / 4) + i, (sdl.SCREEN_WIDTH / 2) -  (i * 2) , (sdl.SCREEN_HEIGHT / 2) - (i * 2) };
					SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 255, 255);
					SDL_RenderDrawRect(sdl.gRenderer, &outlineRect);
				}

				//Render current frame
				SDL_Rect* currentClip = &sdl.gSpriteClips[frame / 4];
				sdl.render((sdl.SCREEN_WIDTH - currentClip->w) / 2, (sdl.SCREEN_HEIGHT - currentClip->h) / 2, sdl.gSpriteSheetTexture, currentClip);

				//Update screen
				SDL_RenderPresent(sdl.gRenderer);

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
