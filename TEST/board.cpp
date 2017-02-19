#include "board.h"
#include <time.h>
#include <random>

board::board()
{
	for (int y = 0; y < 17; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			cheackboxs[y][x].box.x = 60 + (33 * x);
			cheackboxs[y][x].box.y = 60 + (31 * y);
			cheackboxs[y][x].box.w = 33;
			cheackboxs[y][x].box.h = 31;
			
			cheackboxs[y][x].isFree = true;
			cheackboxs[y][x].isSelected = false;
			cheackboxs[y][x].isBase = false;
		}
	}

	timeDelay = 500;
	nivel = 1;
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

			printf("\nHola bienvenido a puyo puyo\n\n");
			printf("Nivel: %d", nivel);

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}

					//Handle input for the dot
					gamehandleEvent(e);
				}

				bool moved = moveDown();

				if (!moved)
				{
					bool isUpdate = updateSprite();
					
					if (!isUpdate)
					{
						bool isDeletePuyos = checkDeletePuyo();
						
						if (!isDeletePuyos)
						{
							/*newPuyos();
							quit = endGame();*/
						}

						newPuyos();
						quit = endGame();
					}
				}
				
				printBackground(randomBackground);
				printBoard();
								
				//Update screen
				SDL_RenderPresent(sdl.gRenderer);
				SDL_Delay(timeDelay);

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
			if (cheackboxs[y][x].isFree == false)
			{
				SDL_Rect* puyoClip = NULL;
				int sprite = cheackboxs[y][x].sprite;

				switch (cheackboxs[y][x].color)
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

				sdl.render(cheackboxs[y][x].box.x, cheackboxs[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
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
			if (cheackboxs[y][x].isSelected)
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
			if (cheackboxs[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxs[y - 1][x].isSelected)
				{
					//if the puyo base is in the last box on the right
					if (x == 10 && cheackboxs[y][x - 1].isFree)
					{
						//move puyo base to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y][x].sprite;

						//move up puyo to down
						cheackboxs[y][x].isFree =		cheackboxs[y - 1][x].isFree;
						cheackboxs[y][x].isSelected =	cheackboxs[y - 1][x].isSelected;
						cheackboxs[y][x].isBase =		cheackboxs[y - 1][x].isBase;
						cheackboxs[y][x].color =			cheackboxs[y - 1][x].color;
						cheackboxs[y][x].sprite =		cheackboxs[y - 1][x].sprite;

						//delete up puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;
						cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x].sprite;

						break;

					}
					else if (cheackboxs[y][x + 1].isFree)
					{
						// up puyo to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y - 1][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y - 1][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y - 1][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y - 1][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y - 1][x].sprite;

						//delete up puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;
						cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x].sprite;
					}
				}
				
				//if the second puyo is on the right
				else if (cheackboxs[y][x + 1].isSelected)
				{
					if (cheackboxs[y + 1][x].isFree && y != 15 )
					{
						// right puyo to dow
						cheackboxs[y + 1][x].isFree =		cheackboxs[y][x + 1].isFree;
						cheackboxs[y + 1][x].isSelected =	cheackboxs[y][x + 1].isSelected;
						cheackboxs[y + 1][x].isBase =		cheackboxs[y][x + 1].isBase;
						cheackboxs[y + 1][x].color =			cheackboxs[y][x + 1].color;
						cheackboxs[y + 1][x].sprite =		cheackboxs[y][x + 1].sprite;

						//delete up puyo
						cheackboxs[y][x + 1].isFree = true;
						cheackboxs[y][x + 1].isSelected = false;
						cheackboxs[y][x + 1].isBase = false;
						cheackboxs[y][x + 1].color;
						cheackboxs[y][x + 1].sprite;

						break;
					}
				}
				
				//if the second puyo is down
				else if (cheackboxs[y + 1][x].isSelected)
				{
					//if the puyo base is in the first box on the left
					if (x == 0 && cheackboxs[y][x + 1].isFree)
					{
						//move puyo base to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y][x].sprite;

						//move down puyo to up
						cheackboxs[y][x].isFree =		cheackboxs[y + 1][x].isFree;
						cheackboxs[y][x].isSelected =	cheackboxs[y + 1][x].isSelected;
						cheackboxs[y][x].isBase =		cheackboxs[y + 1][x].isBase;
						cheackboxs[y][x].color =			cheackboxs[y + 1][x].color;
						cheackboxs[y][x].sprite =		cheackboxs[y + 1][x].sprite;

						//delete up puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;
						cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x].sprite;

						break;

					}
					else if (cheackboxs[y][x - 1].isFree)
					{
						// move down puyo to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y + 1][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y + 1][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y + 1][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y + 1][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y + 1][x].sprite;

						//delete up puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;
						cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x].sprite;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxs[y][x - 1].isSelected)
				{
					if (cheackboxs[y - 1][x].isFree)
					{
						// left puyo to up
						cheackboxs[y - 1][x].isFree =		cheackboxs[y][x - 1].isFree;
						cheackboxs[y - 1][x].isSelected =	cheackboxs[y][x - 1].isSelected;
						cheackboxs[y - 1][x].isBase =		cheackboxs[y][x - 1].isBase;
						cheackboxs[y - 1][x].color =			cheackboxs[y][x - 1].color;
						cheackboxs[y - 1][x].sprite =		cheackboxs[y][x - 1].sprite;

						//delete up puyo
						cheackboxs[y][x - 1].isFree = true;
						cheackboxs[y][x - 1].isSelected = false;
						cheackboxs[y][x - 1].isBase = false;
						cheackboxs[y][x - 1].color;
						cheackboxs[y][x - 1].sprite;

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
			if (cheackboxs[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxs[y - 1][x].isSelected)
				{
					if (x < 10 && cheackboxs[y][x + 1].isFree && cheackboxs[y - 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;
						
						// move the second puyo to right
						cheackboxs[y - 1][x + 1].isFree =		cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][x + 1].isSelected =	cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][x + 1].isBase =		cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][x + 1].color =			cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x + 1].sprite =		cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxs[y][x + 1].isSelected)
				{
					if (x < 9 && cheackboxs[y][x + 2].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][x + 2].isFree =		cheackboxs[y][x + 1].isFree;
						cheackboxs[y][x + 2].isSelected =	cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][x + 2].isBase =		cheackboxs[y][x + 1].isBase;
						cheackboxs[y][x + 2].color =			cheackboxs[y][x + 1].color;
						cheackboxs[y][x + 2].sprite =		cheackboxs[y][x + 1].sprite;

						// move the puyo base to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y][x].sprite;
						
						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxs[y + 1][x].isSelected)
				{
					if (x < 10 && cheackboxs[y][x + 1].isFree && cheackboxs[y + 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][x + 1].isFree =		cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][x + 1].isSelected =	cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][x + 1].isBase =		cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][x + 1].color =			cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x + 1].sprite =		cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxs[y][x - 1].isSelected)
				{
					if (x < 10 && cheackboxs[y][x + 1].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][x + 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite =		cheackboxs[y][x].sprite;

						// move the puyo base to right
						cheackboxs[y][x].isFree =		cheackboxs[y][x - 1].isFree;
						cheackboxs[y][x].isSelected =	cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][x].isBase =		cheackboxs[y][x - 1].isBase;
						cheackboxs[y][x].color =			cheackboxs[y][x - 1].color;
						cheackboxs[y][x].sprite =		cheackboxs[y][x - 1].sprite;

						//delete second puyo
						cheackboxs[y][x - 1].isFree = true;
						cheackboxs[y][x - 1].isSelected = false;
						cheackboxs[y][x - 1].isBase = false;

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
			if (cheackboxs[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxs[y - 1][x].isSelected)
				{
					if (x > 0 && cheackboxs[y][x - 1].isFree && cheackboxs[y - 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to left
						cheackboxs[y - 1][x - 1].isFree =		cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][x - 1].isSelected =	cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][x - 1].isBase =		cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][x - 1].color =			cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x - 1].sprite =		cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxs[y][x + 1].isSelected)
				{
					if (x > 0 && cheackboxs[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y][x].sprite;

						// move the second puyo to left
						cheackboxs[y][x].isFree =		cheackboxs[y][x + 1].isFree;
						cheackboxs[y][x].isSelected =	cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][x].isBase =		cheackboxs[y][x + 1].isBase;
						cheackboxs[y][x].color =			cheackboxs[y][x + 1].color;
						cheackboxs[y][x].sprite =		cheackboxs[y][x + 1].sprite;

						//delete the second puyo 
						cheackboxs[y][x + 1].isFree = true;
						cheackboxs[y][x + 1].isSelected = false;
						cheackboxs[y][x + 1].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxs[y + 1][x].isSelected)
				{
					if (x > 0  && cheackboxs[y][x - 1].isFree && cheackboxs[y + 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][x - 1].isFree =		cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][x - 1].isSelected =	cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][x - 1].isBase =		cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][x - 1].color =			cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x - 1].sprite =		cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxs[y][x - 1].isSelected)
				{
					if (x > 1 && cheackboxs[y][x - 2].isFree)
					{
						// move the second puyo to left
						cheackboxs[y][x - 2].isFree =		cheackboxs[y][x - 1].isFree;
						cheackboxs[y][x - 2].isSelected =	cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][x - 2].isBase =		cheackboxs[y][x - 1].isBase;
						cheackboxs[y][x - 2].color =			cheackboxs[y][x - 1].color;
						cheackboxs[y][x - 2].sprite =		cheackboxs[y][x - 1].sprite;

						// move the puyo base to left
						cheackboxs[y][x - 1].isFree =		cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected =	cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase =		cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color =			cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite =		cheackboxs[y][x].sprite;

						//delete second puyo
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

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
			if (!cheackboxs[y][x].isFree && cheackboxs[y + 1][x].isFree)
			{
				cheackboxs[y + 1][x].isFree =		cheackboxs[y][x].isFree;
				cheackboxs[y + 1][x].isSelected =	cheackboxs[y][x].isSelected;
				cheackboxs[y + 1][x].isBase =		cheackboxs[y][x].isBase;
				cheackboxs[y + 1][x].color =			cheackboxs[y][x].color;
				cheackboxs[y + 1][x].sprite =		cheackboxs[y][x].sprite;

				cheackboxs[y][x].isFree = true;
				cheackboxs[y][x].isSelected = false;
				cheackboxs[y][x].isBase = false;
				
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
			if (cheackboxs[y][x].isSelected)
			{
				cheackboxs[y][x].isSelected = false;
				cheackboxs[y][x].isBase = false;
			}
		}
	}

	cheackboxs[1][5].isSelected = true;
	cheackboxs[1][5].isFree = false;
	cheackboxs[1][5].isBase = true;
	cheackboxs[1][5].sprite = 0;
	int randomColor = rand() % 6;
	cheackboxs[1][5].color = randomColor;

	cheackboxs[0][5].isSelected = true;
	cheackboxs[0][5].isFree = false;
	cheackboxs[0][5].isBase = false;
	cheackboxs[0][5].sprite = 0;
	randomColor = rand() % 6;
	cheackboxs[0][5].color = randomColor;	
}

bool board::endGame()
{
	bool endGame = false;

	if (cheackboxs[1][5].isBase && !cheackboxs[2][5].isFree)
	{
		endGame = true;
	}

	return endGame;
}

bool board::updateSprite()
{
	bool success = false;
	
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (!cheackboxs[y][x].isFree)
			{
				int color = cheackboxs[y][x].color;
				bool up = false, down = false, left = false, right = false;
				
				// check up 
				if (!cheackboxs[y - 1][x].isFree && cheackboxs[y - 1][x].color == color)
				{
					up = true;
				}

				//check down
				if (!cheackboxs[y + 1][x].isFree && cheackboxs[y + 1][x].color == color)
				{
					down = true;
				}

				//check left
				if (!cheackboxs[y][x - 1].isFree && cheackboxs[y][x - 1].color == color)
				{
					left = true;
				}

				//check right
				if (!cheackboxs[y][x + 1].isFree && cheackboxs[y][x + 1].color == color)
				{
					right = true;
				}

				if (up && down && left && right)
				{
					if (cheackboxs[y][x].sprite != 15)
					{
						cheackboxs[y][x].sprite = 15;
						success = true;
					}
				}
				else if (up && left && right)
				{
					if (cheackboxs[y][x].sprite != 14)
					{
						cheackboxs[y][x].sprite = 14;
						success = true;
					}
				}
				else if (down && left && right)
				{
					if (cheackboxs[y][x].sprite != 13)
					{
						cheackboxs[y][x].sprite = 13;
						success = true;
					}
				}
				else if (up && down && left)
				{
					if (cheackboxs[y][x].sprite != 11)
					{
						cheackboxs[y][x].sprite = 11;
						success = true;
					}
				}
				else if (up && down && right)
				{
					if (cheackboxs[y][x].sprite != 7)
					{
						cheackboxs[y][x].sprite = 7;
						success = true;
					}
				}
				else if (up && down)
				{
					if (cheackboxs[y][x].sprite != 3)
					{
						cheackboxs[y][x].sprite = 3;
						success = true;
					}
				}
				else if (up && left)
				{
					if (cheackboxs[y][x].sprite != 10)
					{
						cheackboxs[y][x].sprite = 10;
						success = true;
					}
				}
				else if (up && right)
				{
					if (cheackboxs[y][x].sprite != 6)
					{
						cheackboxs[y][x].sprite = 6;
						success = true;
					}
				}
				else if (down && left)
				{
					if (cheackboxs[y][x].sprite != 9)
					{
						cheackboxs[y][x].sprite = 9;
						success = true;
					}
				}
				else if (down && right)
				{
					if (cheackboxs[y][x].sprite != 5)
					{
						cheackboxs[y][x].sprite = 5;
						success = true;
					}
				}
				else if (left && right)
				{
					if (cheackboxs[y][x].sprite != 12)
					{
						cheackboxs[y][x].sprite = 12;
						success = true;
					}
				}
				else if (up)
				{
					if (cheackboxs[y][x].sprite != 2)
					{
						cheackboxs[y][x].sprite = 2;
						success = true;
					}
				}
				else if (down)
				{
					if (cheackboxs[y][x].sprite != 1)
					{
						cheackboxs[y][x].sprite = 1;
						success = true;
					}
				}
				else if (left)
				{
					if (cheackboxs[y][x].sprite != 8)
					{
						cheackboxs[y][x].sprite = 8;
						success = true;
					}
				}
				else if (right)
				{
					if (cheackboxs[y][x].sprite != 4)
					{
						cheackboxs[y][x].sprite = 4;
						success = true;
					}
				}
				else
				{
					if (cheackboxs[y][x].sprite != 0)
					{
						cheackboxs[y][x].sprite = 0;
						success = true;
					}
				}
			}
		}
	}

	return success;
}

bool board::checkDeletePuyo()
{
	bool success = false;

	for (int y = 16; y > -1; y--)
	{
		for (int x = 10; x > -1; x--)
		{
			if (!cheackboxs[y][x].isFree)
			{
				//delete array with value to delete puyos
				for (int i = 0; i < 10; i++)
				{
					deletesPuyos[i].x = 0;
					deletesPuyos[i].y = 0;
					deletesPuyos[i].empty = true;
				}

				deletesPuyos[0].x = x;
				deletesPuyos[0].y = y;
				deletesPuyos[0].empty = false;

				deletePuyos(y, x, NULL, NULL);
				
				

				if (!deletesPuyos[3].empty)
				{
					//delete puyos
					for (int i = 0; i < 10; i++)
					{
						if (!deletesPuyos[i].empty)
						{
							cheackboxs[deletesPuyos[i].y][deletesPuyos[i].x].isFree = true;
							cheackboxs[deletesPuyos[i].y][deletesPuyos[i].x].isBase = false;
							cheackboxs[deletesPuyos[i].y][deletesPuyos[i].x].isSelected = false;
							
							updateSprite();
						}
					}

					nivel = nivel + 1;
					printf("\nNivel: %d", nivel);
					
					timeDelay = timeDelay - 15;
					success = true;
				}
			}
		}
	}

	return success;
}

void board::deletePuyos(int y, int x, int originX, int originY)
{
	int color = cheackboxs[y][x].color;
	
	//up
	if (!cheackboxs[y - 1][x].isFree && cheackboxs[y - 1][x].color == color && !compareBoxes(cheackboxs[y - 1][x],cheackboxs[originY][originX]))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty) 
			{
				deletesPuyos[i].x = x;
				deletesPuyos[i].y = y - 1;
				deletesPuyos[i].empty = false;
				break;
			}
		}

		deletePuyos(y - 1, x, x, y);
	}
	
	//down
	if (!cheackboxs[y + 1][x].isFree && cheackboxs[y + 1][x].color == color && !compareBoxes(cheackboxs[y + 1][x], cheackboxs[originY][originX]))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x;
				deletesPuyos[i].y = y + 1;
				deletesPuyos[i].empty = false;
				break;
			}
		}

		deletePuyos(y + 1, x, x, y);
	}
	
	//left
	if (!cheackboxs[y][x - 1].isFree && cheackboxs[y][x - 1].color == color && !compareBoxes(cheackboxs[y][x - 1], cheackboxs[originY][originX]))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x - 1;
				deletesPuyos[i].y = y;
				deletesPuyos[i].empty = false;
				break;
			}
		}

		deletePuyos(y, x - 1, x, y);
	}
	
	//right
	if (!cheackboxs[y][x + 1].isFree && cheackboxs[y][x + 1].color == color && !compareBoxes(cheackboxs[y][x + 1], cheackboxs[originY][originX]))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x + 1;
				deletesPuyos[i].y = y;
				deletesPuyos[i].empty = false;
				break;
			}
		}

		deletePuyos(y + 1, x, x, y);
	}
}

bool board::compareBoxes(cheackbox box1, cheackbox box2)
{
	bool success = false;
	
	if (box1.box.x == box2.box.x && box1.box.y == box2.box.y)
	{
		success = true;
	}

	return success;
}