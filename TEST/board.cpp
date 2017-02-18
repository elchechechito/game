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
			cheackbox[y][x].isBase = false;
		}
	}

	cheackbox[1][5].isSelected = true;
	cheackbox[1][5].isBase = true;
	cheackbox[1][5].isFree = false;
	cheackbox[1][5].color = 0;
	cheackbox[1][5].sprite = 0;

	cheackbox[0][5].isSelected = true;
	cheackbox[0][5].isBase = false;
	cheackbox[0][5].isFree = false;
	cheackbox[0][5].color = 0;
	cheackbox[0][5].sprite = 0;
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

				bool moved = moveDown();

				if (!moved)
				{
					newPuyos();
				}

				printBackground(randomBackground);
				printBoard();
								
				//Update screen
				SDL_RenderPresent(sdl.gRenderer);
				SDL_Delay(200);

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

	//verificar si hay casillas seleccionadas
		//si
				//verificar si se toco la tecla UP para girar
				// se puede girar?
					//si - girar el puyo
		
			//verificar si esta abajo hay otro puyo
				// si - cambiar el estado de isSelected a falso
				// no - bajar puyo a la casilla
		//no
			//verificar si abajo hay otro puyo
				//si - bajar casillas
				//no 
					//verificar si se deben de actualizar los sprites
						//si - actualizar sprite
						//no
							//verificar si se deben de destruir los puyos
								//si - destruir si hay 4 puyos del mismo color
								//no - crear nuevos puyos seleccionables

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
		//turn second puyo
		switch (e.key.keysym.sym)
		{
			case SDLK_UP: 
				turnPuyo();
				break;

			case SDLK_RIGHT:
				moveRight();
				break;

			case SDLK_LEFT:
				moveLeft();
				break;
		}
	}
}

void board::turnPuyo()
{
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			//search the puyo base
			if (cheackbox[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackbox[y - 1][x].isSelected)
				{
					//if the puyo base is in the last box on the right
					if (x == 10 && cheackbox[y][x - 1].isFree)
					{
						//move puyo base to left
						cheackbox[y][x - 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y][x].sprite;

						//move up puyo to down
						cheackbox[y][x].isFree =		cheackbox[y - 1][x].isFree;
						cheackbox[y][x].isSelected =	cheackbox[y - 1][x].isSelected;
						cheackbox[y][x].isBase =		cheackbox[y - 1][x].isBase;
						cheackbox[y][x].color =			cheackbox[y - 1][x].color;
						cheackbox[y][x].sprite =		cheackbox[y - 1][x].sprite;

						//delete up puyo
						cheackbox[y - 1][x].isFree = true;
						cheackbox[y - 1][x].isSelected = false;
						cheackbox[y - 1][x].isBase = false;
						cheackbox[y - 1][x].color;
						cheackbox[y - 1][x].sprite;

						break;

					}
					else if (cheackbox[y][x + 1].isFree)
					{
						// up puyo to right
						cheackbox[y][x + 1].isFree =		cheackbox[y - 1][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y - 1][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y - 1][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y - 1][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y - 1][x].sprite;

						//delete up puyo
						cheackbox[y - 1][x].isFree = true;
						cheackbox[y - 1][x].isSelected = false;
						cheackbox[y - 1][x].isBase = false;
						cheackbox[y - 1][x].color;
						cheackbox[y - 1][x].sprite;
					}
				}
				
				//if the second puyo is on the right
				else if (cheackbox[y][x + 1].isSelected)
				{
					if (cheackbox[y + 1][x].isFree)
					{
						// right puyo to dow
						cheackbox[y + 1][x].isFree =		cheackbox[y][x + 1].isFree;
						cheackbox[y + 1][x].isSelected =	cheackbox[y][x + 1].isSelected;
						cheackbox[y + 1][x].isBase =		cheackbox[y][x + 1].isBase;
						cheackbox[y + 1][x].color =			cheackbox[y][x + 1].color;
						cheackbox[y + 1][x].sprite =		cheackbox[y][x + 1].sprite;

						//delete up puyo
						cheackbox[y][x + 1].isFree = true;
						cheackbox[y][x + 1].isSelected = false;
						cheackbox[y][x + 1].isBase = false;
						cheackbox[y][x + 1].color;
						cheackbox[y][x + 1].sprite;

						break;
					}
				}
				
				//if the second puyo is down
				else if (cheackbox[y + 1][x].isSelected)
				{
					//if the puyo base is in the first box on the left
					if (x == 0 && cheackbox[y][x + 1].isFree)
					{
						//move puyo base to right
						cheackbox[y][x + 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y][x].sprite;

						//move down puyo to up
						cheackbox[y][x].isFree =		cheackbox[y + 1][x].isFree;
						cheackbox[y][x].isSelected =	cheackbox[y + 1][x].isSelected;
						cheackbox[y][x].isBase =		cheackbox[y + 1][x].isBase;
						cheackbox[y][x].color =			cheackbox[y + 1][x].color;
						cheackbox[y][x].sprite =		cheackbox[y + 1][x].sprite;

						//delete up puyo
						cheackbox[y + 1][x].isFree = true;
						cheackbox[y + 1][x].isSelected = false;
						cheackbox[y + 1][x].isBase = false;
						cheackbox[y + 1][x].color;
						cheackbox[y + 1][x].sprite;

						break;

					}
					else if (cheackbox[y][x - 1].isFree)
					{
						// move down puyo to left
						cheackbox[y][x - 1].isFree =		cheackbox[y + 1][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y + 1][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y + 1][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y + 1][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y + 1][x].sprite;

						//delete up puyo
						cheackbox[y + 1][x].isFree = true;
						cheackbox[y + 1][x].isSelected = false;
						cheackbox[y + 1][x].isBase = false;
						cheackbox[y + 1][x].color;
						cheackbox[y + 1][x].sprite;
					}
				}

				//if the second puyo is on the left
				else if (cheackbox[y][x - 1].isSelected)
				{
					if (cheackbox[y - 1][x].isFree)
					{
						// left puyo to up
						cheackbox[y - 1][x].isFree =		cheackbox[y][x - 1].isFree;
						cheackbox[y - 1][x].isSelected =	cheackbox[y][x - 1].isSelected;
						cheackbox[y - 1][x].isBase =		cheackbox[y][x - 1].isBase;
						cheackbox[y - 1][x].color =			cheackbox[y][x - 1].color;
						cheackbox[y - 1][x].sprite =		cheackbox[y][x - 1].sprite;

						//delete up puyo
						cheackbox[y][x - 1].isFree = true;
						cheackbox[y][x - 1].isSelected = false;
						cheackbox[y][x - 1].isBase = false;
						cheackbox[y][x - 1].color;
						cheackbox[y][x - 1].sprite;

						break;
					}

				}

			}
		}
	}
}

void board::moveRight()
{
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			//find the puyo base
			if (cheackbox[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackbox[y - 1][x].isSelected)
				{
					if (x < 10 && cheackbox[y][x + 1].isFree && cheackbox[y - 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackbox[y][x + 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y][x].sprite;

						//delete puyo base
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;
						
						// move the second puyo to right
						cheackbox[y - 1][x + 1].isFree =		cheackbox[y - 1][x].isFree;
						cheackbox[y - 1][x + 1].isSelected =	cheackbox[y - 1][x].isSelected;
						cheackbox[y - 1][x + 1].isBase =		cheackbox[y - 1][x].isBase;
						cheackbox[y - 1][x + 1].color =			cheackbox[y - 1][x].color;
						cheackbox[y - 1][x + 1].sprite =		cheackbox[y - 1][x].sprite;

						//delete second puyo
						cheackbox[y - 1][x].isFree = true;
						cheackbox[y - 1][x].isSelected = false;
						cheackbox[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackbox[y][x + 1].isSelected)
				{
					if (x < 9 && cheackbox[y][x + 2].isFree)
					{
						// move the second puyo to right
						cheackbox[y][x + 2].isFree =		cheackbox[y][x + 1].isFree;
						cheackbox[y][x + 2].isSelected =	cheackbox[y][x + 1].isSelected;
						cheackbox[y][x + 2].isBase =		cheackbox[y][x + 1].isBase;
						cheackbox[y][x + 2].color =			cheackbox[y][x + 1].color;
						cheackbox[y][x + 2].sprite =		cheackbox[y][x + 1].sprite;

						// move the puyo base to right
						cheackbox[y][x + 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y][x].sprite;
						
						//delete puyo base
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackbox[y + 1][x].isSelected)
				{
					if (x < 10 && cheackbox[y][x + 1].isFree && cheackbox[y + 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackbox[y][x + 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y][x].sprite;

						//delete puyo base
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;

						// move the second puyo to right
						cheackbox[y + 1][x + 1].isFree =		cheackbox[y + 1][x].isFree;
						cheackbox[y + 1][x + 1].isSelected =	cheackbox[y + 1][x].isSelected;
						cheackbox[y + 1][x + 1].isBase =		cheackbox[y + 1][x].isBase;
						cheackbox[y + 1][x + 1].color =			cheackbox[y + 1][x].color;
						cheackbox[y + 1][x + 1].sprite =		cheackbox[y + 1][x].sprite;

						//delete second puyo
						cheackbox[y + 1][x].isFree = true;
						cheackbox[y + 1][x].isSelected = false;
						cheackbox[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackbox[y][x - 1].isSelected)
				{
					if (x < 10 && cheackbox[y][x + 1].isFree)
					{
						// move the second puyo to right
						cheackbox[y][x + 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x + 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x + 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x + 1].color =			cheackbox[y][x].color;
						cheackbox[y][x + 1].sprite =		cheackbox[y][x].sprite;

						// move the puyo base to right
						cheackbox[y][x].isFree =		cheackbox[y][x - 1].isFree;
						cheackbox[y][x].isSelected =	cheackbox[y][x - 1].isSelected;
						cheackbox[y][x].isBase =		cheackbox[y][x - 1].isBase;
						cheackbox[y][x].color =			cheackbox[y][x - 1].color;
						cheackbox[y][x].sprite =		cheackbox[y][x - 1].sprite;

						//delete second puyo
						cheackbox[y][x - 1].isFree = true;
						cheackbox[y][x - 1].isSelected = false;
						cheackbox[y][x - 1].isBase = false;

						break;
					}
				}

			}
		}
	}
}

void board::moveLeft()
{
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			//find the puyo base
			if (cheackbox[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackbox[y - 1][x].isSelected)
				{
					if (x > 0 && cheackbox[y][x - 1].isFree && cheackbox[y - 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackbox[y][x - 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y][x].sprite;

						//delete puyo base
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;

						// move the second puyo to left
						cheackbox[y - 1][x - 1].isFree =		cheackbox[y - 1][x].isFree;
						cheackbox[y - 1][x - 1].isSelected =	cheackbox[y - 1][x].isSelected;
						cheackbox[y - 1][x - 1].isBase =		cheackbox[y - 1][x].isBase;
						cheackbox[y - 1][x - 1].color =			cheackbox[y - 1][x].color;
						cheackbox[y - 1][x - 1].sprite =		cheackbox[y - 1][x].sprite;

						//delete second puyo
						cheackbox[y - 1][x].isFree = true;
						cheackbox[y - 1][x].isSelected = false;
						cheackbox[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackbox[y][x + 1].isSelected)
				{
					if (x > 0 && cheackbox[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackbox[y][x - 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y][x].sprite;

						// move the second puyo to left
						cheackbox[y][x].isFree =		cheackbox[y][x + 1].isFree;
						cheackbox[y][x].isSelected =	cheackbox[y][x + 1].isSelected;
						cheackbox[y][x].isBase =		cheackbox[y][x + 1].isBase;
						cheackbox[y][x].color =			cheackbox[y][x + 1].color;
						cheackbox[y][x].sprite =		cheackbox[y][x + 1].sprite;

						//delete the second puyo 
						cheackbox[y][x + 1].isFree = true;
						cheackbox[y][x + 1].isSelected = false;
						cheackbox[y][x + 1].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackbox[y + 1][x].isSelected)
				{
					if (x > 0  && cheackbox[y][x - 1].isFree && cheackbox[y + 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackbox[y][x - 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y][x].sprite;

						//delete puyo base
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;

						// move the second puyo to right
						cheackbox[y + 1][x - 1].isFree =		cheackbox[y + 1][x].isFree;
						cheackbox[y + 1][x - 1].isSelected =	cheackbox[y + 1][x].isSelected;
						cheackbox[y + 1][x - 1].isBase =		cheackbox[y + 1][x].isBase;
						cheackbox[y + 1][x - 1].color =			cheackbox[y + 1][x].color;
						cheackbox[y + 1][x - 1].sprite =		cheackbox[y + 1][x].sprite;

						//delete second puyo
						cheackbox[y + 1][x].isFree = true;
						cheackbox[y + 1][x].isSelected = false;
						cheackbox[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackbox[y][x - 1].isSelected)
				{
					if (x > 1 && cheackbox[y][x - 2].isFree)
					{
						// move the second puyo to left
						cheackbox[y][x - 2].isFree =		cheackbox[y][x - 1].isFree;
						cheackbox[y][x - 2].isSelected =	cheackbox[y][x - 1].isSelected;
						cheackbox[y][x - 2].isBase =		cheackbox[y][x - 1].isBase;
						cheackbox[y][x - 2].color =			cheackbox[y][x - 1].color;
						cheackbox[y][x - 2].sprite =		cheackbox[y][x - 1].sprite;

						// move the puyo base to left
						cheackbox[y][x - 1].isFree =		cheackbox[y][x].isFree;
						cheackbox[y][x - 1].isSelected =	cheackbox[y][x].isSelected;
						cheackbox[y][x - 1].isBase =		cheackbox[y][x].isBase;
						cheackbox[y][x - 1].color =			cheackbox[y][x].color;
						cheackbox[y][x - 1].sprite =		cheackbox[y][x].sprite;

						//delete second puyo
						cheackbox[y][x].isFree = true;
						cheackbox[y][x].isSelected = false;
						cheackbox[y][x].isBase = false;

						break;
					}
				}

			}
		}
	}
}

bool board::moveDown()
{
	bool success = false;
	
	for (int y = 14; y > -1; y--)
	{
		for (int x = 10; x > -1; x--)
		{
			if (!cheackbox[y][x].isFree && cheackbox[y + 1][x].isFree)
			{
				cheackbox[y + 1][x].isFree =		cheackbox[y][x].isFree;
				cheackbox[y + 1][x].isSelected =	cheackbox[y][x].isSelected;
				cheackbox[y + 1][x].isBase =		cheackbox[y][x].isBase;
				cheackbox[y + 1][x].color =			cheackbox[y][x].color;
				cheackbox[y + 1][x].sprite =		cheackbox[y][x].sprite;

				cheackbox[y][x].isFree = true;
				cheackbox[y][x].isSelected = false;
				cheackbox[y][x].isBase = false;
				
				success = true;
								
			}
		}
	}

	return success;
}

void board::newPuyos()
{
	//unselect puyos
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (cheackbox[y][x].isSelected)
			{
				cheackbox[y][x].isSelected = false;
				cheackbox[y][x].isBase = false;
			}
		}
	}

	cheackbox[1][5].isSelected = true;
	cheackbox[1][5].isFree = false;
	cheackbox[1][5].isBase = true;
	cheackbox[1][5].sprite = 0;

	int randomColor = rand() % 5;

	cheackbox[1][5].color = randomColor;

	cheackbox[0][5].isSelected = true;
	cheackbox[0][5].isFree = false;
	cheackbox[0][5].isBase = false;
	cheackbox[0][5].sprite = 0;

	randomColor = rand() % 5;

	cheackbox[0][5].color = randomColor;




}
