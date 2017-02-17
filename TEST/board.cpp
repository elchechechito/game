#include "board.h"
#include <time.h>
#include <random>

board::board()
{
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			cheackbox[y][x].box.x = 60 + (33 * x);
			cheackbox[y][x].box.y = 60 + (31 * y);
			cheackbox[y][x].box.w = 33;
			cheackbox[y][x].box.h = 31;
			
			cheackbox[y][x].isFree = true;
			cheackbox[y][x].isSelected = false;
		}
	}
}


board::~board()
{
}

void board::startGameBoard()
{
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

			board theBoard;

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

					//Handle input for the dot
					gamehandleEvent(e);
				}

				printBackground(randomBackground);
				printBoard();
								
				//Update screen
				SDL_RenderPresent(sdl.gRenderer);
				SDL_Delay(1500);

			}
		}
	}
}

void board::printBackground(int randomBackground)
{
	//Clear screen
	SDL_SetRenderDrawColor(sdl.gRenderer, 255, 255, 255, 0xFF);
	SDL_RenderClear(sdl.gRenderer);

	//render the background
	SDL_Rect* backgroundClip = &sdl.backgroundSprite[randomBackground];
	sdl.render(0, 0, sdl.gTextureBackground, backgroundClip, true);

	// add belnd mode to render
	SDL_SetRenderDrawBlendMode(sdl.gRenderer, SDL_BLENDMODE_BLEND);

	//Render red filled quad
	SDL_Rect fillRect = { 50, 50, 390, 515 };
	SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 150);
	SDL_RenderFillRect(sdl.gRenderer, &fillRect);
}

void board::printBoard()
{
	//print the board
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (cheackbox[y][x].isFree == false)
			{
				SDL_Rect* puyoClip = NULL;
				int sprite = cheackbox[y][x].sprite;

				switch (cheackbox[y][x].color)
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

				sdl.render(cheackbox[y][x].box.x, cheackbox[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
			}
		}
	}
}

void board::gamehandleEvent(SDL_Event & e)
{
	if (cheackboxIsSelected())
	{
		moveCheackbox( e );		
	}
	else
	{

	}

	//checar si hay una casilla seleccionada
	//bajar o girar dependiendo si hay un evento 

	//si no hay casillas seleccionadas 
	//verificar si se tienen que eliminar los puyos

	//si no se tiene que eliminar ningun puyo se debe de crear dos nuevo en la casilla 5,1 y 5,0
}

bool board::cheackboxIsSelected()
{
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (cheackbox[y][x].isSelected)
			{
				return true;
			}
		}
	}
	
	return false;
}

void board::moveCheackbox(SDL_Event & e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
			case SDLK_UP: 
				turnPuyo();
				break;			
		}
	}

	moveDown();
}

void board::turnPuyo()
{

}
