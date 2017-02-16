//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "loadSDL.h"
#include <time.h>
#include "board.h"

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

			board theBoard;

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
				SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 150);
				SDL_RenderFillRect(sdl.gRenderer, &fillRect);
				
				for (int i = 0; i < 10; i++)
				{
					//Render green outlined quad
					SDL_Rect outlineRect = { 50 + i , 50 + i, 390 - (i * 2), 515 - (i * 2) };
					SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 255, 255);
					SDL_RenderDrawRect(sdl.gRenderer, &outlineRect);
				}
				
				//print the board
				for (int y = 0; y < 16; y++)
				{
					for (int x = 0; x < 11; x++)
					{
						if (theBoard.cheackbox[y][x].isFree == false)
						{
							SDL_Rect* puyoClip = NULL;
							int sprite = theBoard.cheackbox[y][x].sprite;

							switch (theBoard.cheackbox[y][x].color)
							{
							case 0:
								puyoClip = &sdl.redPuyoSprite[sprite];
								break;

							case 1:
								puyoClip = &sdl.greenPuyoSprite[sprite];
								break;

							case 2:
								puyoClip = &sdl.bluePuyoSprite[sprite];
								break;

							case 3:
								puyoClip = &sdl.yellowPuyoSprite[sprite];
								break;

							case 4:
								puyoClip = &sdl.purplePuyoSprite[sprite];
								break;

							case 5:
								puyoClip = &sdl.pinkPuyoSprite[sprite];
								break;
							}

							sdl.render(theBoard.cheackbox[y][x].box.x, theBoard.cheackbox[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
						}												
					}
				}
				
				//Update screen
				SDL_RenderPresent(sdl.gRenderer);
				SDL_Delay(1500);

			}
		}
	}

	//Free resources and close SDL
	sdl.close();

	return 0;
}
