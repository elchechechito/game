#include "board.h"
#include <time.h>
#include <random>

board::board()
{
	m_timestamp = 0;
	changeDown = false;

	rows = 16;
	columns = 11;

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			cheackboxs[y][x].box.x = 40 + (33 * x);
			cheackboxs[y][x].box.y = 40 + (31 * y);
			cheackboxs[y][x].box.w = 33;
			cheackboxs[y][x].box.h = 31;

			cheackboxs[y][x].isFree = true;
			cheackboxs[y][x].isSelected = false;
			cheackboxs[y][x].isBase = false;
			cheackboxs[y][x].color = 10;
		}
	}

	srand(time(0));

	for (int i = 0; i < 3; i++)
	{
		int randomX = rand() % columns;
		int randomY = rand() % rows;

		cheackboxs[randomY][randomX].isFree = false;
		cheackboxs[randomY][randomX].isSelected = false;
		cheackboxs[randomY][randomX].isBase = false;
		cheackboxs[randomY][randomX].color = 6;
	}

	timeDelay = 500;
	combos = 0;
}

board::~board()
{
}

void board::startGameBoard()
{
	//time start
	m_timestamp = SDL_GetTicks();
	
	//Start SDL, SDL_Image
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

			//Pause flag
			bool pause = false;

			//Background
			int randomBackground = rand() % 4;

			//Show welcome message and combos
			system("CLS");
			printf("\n Hello Welcome to Puyo Puyo\n\n");
			printf("Combos: %d", combos);

			//While application is running
			while (!quit)
			{
				//validation if the new puyos 
				quit = endGame();

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//Player requests quit
					if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}

					//Press P to pause
					if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
					{
						pause = !pause;
					}

					//Press F to flip
					if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_f)
					{
						flipBoard();
					}

					//Don't handler movement if is on pause
					if (!pause)
					{
						//Handle input
						gamehandleEvent(e);
					}
				}

				int elapsed_time = SDL_GetTicks() - m_timestamp; 
				if (elapsed_time > timeDelay)
				{
					if (!pause)
					{
						m_timestamp = SDL_GetTicks();
						
						//move down or up all piece
						bool moved = moveDown();

						if (!moved)
						{
							//Update all sprite
							bool isUpdate = updateSprite();

							if (!isUpdate)
							{
								//Cheack if the puyos were removed
								bool isDeletePuyos = checkDeletePuyo();

								if (!isDeletePuyos)
								{
									newPuyos();
								}
							}
						}
					}
				}
				
				printBackground(randomBackground);
				printBoard();

				//Update screen
				SDL_RenderPresent(sdl.gRenderer);
				SDL_Delay(10);

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
	SDL_Rect fillRect = { 40, 40, columns * 33, rows * 31 };
	SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 150);
	SDL_RenderFillRect(sdl.gRenderer, &fillRect);

	//Render red filled quad player 2
	fillRect = { 500, 40, columns * 33, rows * 31 };
	SDL_SetRenderDrawColor(sdl.gRenderer, 91, 152, 219, 150);
	SDL_RenderFillRect(sdl.gRenderer, &fillRect);
}

void board::printBoard()
{
	//print the board
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
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

				case 6:
					puyoClip = &sdl.badPuyoSprite;
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
		moveCheackbox(e);
	}
}

bool board::cheackboxIsSelected()
{
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
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
	if (e.type == SDL_KEYDOWN /*&& e.key.repeat == 0*/)
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

		case SDLK_SPACE:
			changePuyo();
			break;

		case SDLK_DOWN:
			bool isMoveDown = true;

			while (isMoveDown)
			{
				isMoveDown = moveDown();
			}

			break;
		}
	}
}

void board::turnPuyo()
{
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//search the puyo base
			if (cheackboxs[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxs[y - 1][x].isSelected)
				{
					//if the puyo base is in the last box on the right
					if (x == columns -1 && cheackboxs[y][x - 1].isFree)
					{
						//move puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						//move up puyo to down
						cheackboxs[y][x].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y][x].color = cheackboxs[y - 1][x].color;
						cheackboxs[y][x].sprite = cheackboxs[y - 1][x].sprite;

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
						cheackboxs[y][x + 1].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y - 1][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y - 1][x].sprite;

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
					if (cheackboxs[y + 1][x].isFree && y != rows -1)
					{
						// right puyo to dow
						cheackboxs[y + 1][x].isFree = cheackboxs[y][x + 1].isFree;
						cheackboxs[y + 1][x].isSelected = cheackboxs[y][x + 1].isSelected;
						cheackboxs[y + 1][x].isBase = cheackboxs[y][x + 1].isBase;
						cheackboxs[y + 1][x].color = cheackboxs[y][x + 1].color;
						cheackboxs[y + 1][x].sprite = cheackboxs[y][x + 1].sprite;

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
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						//move down puyo to up
						cheackboxs[y][x].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y][x].color = cheackboxs[y + 1][x].color;
						cheackboxs[y][x].sprite = cheackboxs[y + 1][x].sprite;

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
						cheackboxs[y][x - 1].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y + 1][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y + 1][x].sprite;

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
						cheackboxs[y - 1][x].isFree = cheackboxs[y][x - 1].isFree;
						cheackboxs[y - 1][x].isSelected = cheackboxs[y][x - 1].isSelected;
						cheackboxs[y - 1][x].isBase = cheackboxs[y][x - 1].isBase;
						cheackboxs[y - 1][x].color = cheackboxs[y][x - 1].color;
						cheackboxs[y - 1][x].sprite = cheackboxs[y][x - 1].sprite;

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
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//find the puyo base
			if (cheackboxs[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxs[y - 1][x].isSelected)
				{
					if (x < columns -1 && cheackboxs[y][x + 1].isFree && cheackboxs[y - 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y - 1][x + 1].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][x + 1].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][x + 1].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][x + 1].color = cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x + 1].sprite = cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
					else if (x == columns -1 && cheackboxs[y][0].isFree && cheackboxs[y - 1][0].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][0].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][0].color = cheackboxs[y][x].color;
						cheackboxs[y][0].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y - 1][0].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][0].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][0].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][0].color = cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][0].sprite = cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxs[y + 1][x].isSelected)
				{
					if (x < columns -1 && cheackboxs[y][x + 1].isFree && cheackboxs[y + 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][x + 1].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][x + 1].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][x + 1].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][x + 1].color = cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x + 1].sprite = cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
					else if (x == columns -1 && cheackboxs[y][0].isFree && cheackboxs[y + 1][0].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][0].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][0].color = cheackboxs[y][x].color;
						cheackboxs[y][0].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][0].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][0].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][0].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][0].color = cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][0].sprite = cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxs[y][x - 1].isSelected || (x == 0 && cheackboxs[y][columns-1].isSelected))
				{
					if (x == 0 && cheackboxs[y][x + 1].isFree)
					{
						// move the puyo bas to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						// move the second puyo to right
						cheackboxs[y][0].isFree = cheackboxs[y][columns -1].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][columns - 1].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][columns - 1].isBase;
						cheackboxs[y][0].color = cheackboxs[y][columns - 1].color;
						cheackboxs[y][0].sprite = cheackboxs[y][columns - 1].sprite;

						//delete second puyo
						cheackboxs[y][columns -1].isFree = true;
						cheackboxs[y][columns - 1].isSelected = false;
						cheackboxs[y][columns - 1].isBase = false;

						break;
					}
					else if (x < columns -1 && cheackboxs[y][x + 1].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						// move the puyo base to right
						cheackboxs[y][x].isFree = cheackboxs[y][x - 1].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y][x - 1].isBase;
						cheackboxs[y][x].color = cheackboxs[y][x - 1].color;
						cheackboxs[y][x].sprite = cheackboxs[y][x - 1].sprite;

						//delete second puyo
						cheackboxs[y][x - 1].isFree = true;
						cheackboxs[y][x - 1].isSelected = false;
						cheackboxs[y][x - 1].isBase = false;

						break;
					}
					else if (x == columns -1 && cheackboxs[y][0].isFree)
					{
						// move the puyo base to right
						cheackboxs[y][0].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][0].color = cheackboxs[y][x].color;
						cheackboxs[y][0].sprite = cheackboxs[y][x].sprite;

						// move the second puyo to right
						cheackboxs[y][x].isFree = cheackboxs[y][x - 1].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y][x - 1].isBase;
						cheackboxs[y][x].color = cheackboxs[y][x - 1].color;
						cheackboxs[y][x].sprite = cheackboxs[y][x - 1].sprite;

						//delete second puyo
						cheackboxs[y][x - 1].isFree = true;
						cheackboxs[y][x - 1].isSelected = false;
						cheackboxs[y][x - 1].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxs[y][x + 1].isSelected || (x == columns-1 && cheackboxs[y][0].isSelected))
				{
					if (x < columns -2 && cheackboxs[y][x + 2].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][x + 2].isFree = cheackboxs[y][x + 1].isFree;
						cheackboxs[y][x + 2].isSelected = cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][x + 2].isBase = cheackboxs[y][x + 1].isBase;
						cheackboxs[y][x + 2].color = cheackboxs[y][x + 1].color;
						cheackboxs[y][x + 2].sprite = cheackboxs[y][x + 1].sprite;

						// move the puyo base to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
					else if (x == columns -2 && cheackboxs[y][0].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][0].isFree = cheackboxs[y][x + 1].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][x + 1].isBase;
						cheackboxs[y][0].color = cheackboxs[y][x + 1].color;
						cheackboxs[y][0].sprite = cheackboxs[y][x + 1].sprite;

						// move the puyo base to right
						cheackboxs[y][x + 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x + 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x + 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x + 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x + 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
					else if (x == columns-1 && cheackboxs[y][1].isFree)
					{
						// move the second puyo to right
						cheackboxs[y][1].isFree = cheackboxs[y][0].isFree;
						cheackboxs[y][1].isSelected = cheackboxs[y][0].isSelected;
						cheackboxs[y][1].isBase = cheackboxs[y][0].isBase;
						cheackboxs[y][1].color = cheackboxs[y][0].color;
						cheackboxs[y][1].sprite = cheackboxs[y][0].sprite;

						// move the puyo base to right
						cheackboxs[y][0].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][0].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][0].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][0].color = cheackboxs[y][x].color;
						cheackboxs[y][0].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
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

void board::moveLeft()
{
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
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
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to left
						cheackboxs[y - 1][x - 1].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][x - 1].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][x - 1].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][x - 1].color = cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][x - 1].sprite = cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxs[y][columns -1].isFree && cheackboxs[y - 1][columns -1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][columns -1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][columns - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][columns - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][columns - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][columns - 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to left
						cheackboxs[y - 1][columns - 1].isFree = cheackboxs[y - 1][x].isFree;
						cheackboxs[y - 1][columns - 1].isSelected = cheackboxs[y - 1][x].isSelected;
						cheackboxs[y - 1][columns - 1].isBase = cheackboxs[y - 1][x].isBase;
						cheackboxs[y - 1][columns - 1].color = cheackboxs[y - 1][x].color;
						cheackboxs[y - 1][columns - 1].sprite = cheackboxs[y - 1][x].sprite;

						//delete second puyo
						cheackboxs[y - 1][x].isFree = true;
						cheackboxs[y - 1][x].isSelected = false;
						cheackboxs[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxs[y + 1][x].isSelected)
				{
					if (x > 0 && cheackboxs[y][x - 1].isFree && cheackboxs[y + 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][x - 1].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][x - 1].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][x - 1].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][x - 1].color = cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][x - 1].sprite = cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxs[y][columns -1].isFree && cheackboxs[y + 1][columns -1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][columns - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][columns - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][columns - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][columns - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][columns - 1].sprite = cheackboxs[y][x].sprite;

						//delete puyo base
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						// move the second puyo to right
						cheackboxs[y + 1][columns - 1].isFree = cheackboxs[y + 1][x].isFree;
						cheackboxs[y + 1][columns - 1].isSelected = cheackboxs[y + 1][x].isSelected;
						cheackboxs[y + 1][columns - 1].isBase = cheackboxs[y + 1][x].isBase;
						cheackboxs[y + 1][columns - 1].color = cheackboxs[y + 1][x].color;
						cheackboxs[y + 1][columns - 1].sprite = cheackboxs[y + 1][x].sprite;

						//delete second puyo
						cheackboxs[y + 1][x].isFree = true;
						cheackboxs[y + 1][x].isSelected = false;
						cheackboxs[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxs[y][x - 1].isSelected || (x == 0 && cheackboxs[y][columns - 1].isSelected))
				{
					if (x > 1 && cheackboxs[y][x - 2].isFree)
					{
						// move the second puyo to left
						cheackboxs[y][x - 2].isFree = cheackboxs[y][x - 1].isFree;
						cheackboxs[y][x - 2].isSelected = cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][x - 2].isBase = cheackboxs[y][x - 1].isBase;
						cheackboxs[y][x - 2].color = cheackboxs[y][x - 1].color;
						cheackboxs[y][x - 2].sprite = cheackboxs[y][x - 1].sprite;

						// move the puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						//delete second puyo
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
					else if (x == 1 && cheackboxs[y][columns - 1].isFree)
					{
						// move the second puyo to left
						cheackboxs[y][columns - 1].isFree =		cheackboxs[y][x - 1].isFree;
						cheackboxs[y][columns - 1].isSelected =	cheackboxs[y][x - 1].isSelected;
						cheackboxs[y][columns - 1].isBase =		cheackboxs[y][x - 1].isBase;
						cheackboxs[y][columns - 1].color =		cheackboxs[y][x - 1].color;
						cheackboxs[y][columns - 1].sprite =		cheackboxs[y][x - 1].sprite;

						// move the puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						//delete second puyo
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxs[y][columns - 2].isFree)
					{
						// move the second puyo to left
						cheackboxs[y][columns - 2].isFree =		cheackboxs[y][columns - 1].isFree;
						cheackboxs[y][columns - 2].isSelected =	cheackboxs[y][columns - 1].isSelected;
						cheackboxs[y][columns - 2].isBase =		cheackboxs[y][columns - 1].isBase;
						cheackboxs[y][columns - 2].color =		cheackboxs[y][columns - 1].color;
						cheackboxs[y][columns - 2].sprite =		cheackboxs[y][columns - 1].sprite;

						// move the puyo base to left
						cheackboxs[y][columns - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][columns - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][columns - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][columns - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][columns - 1].sprite = cheackboxs[y][x].sprite;

						//delete second puyo
						cheackboxs[y][x].isFree = true;
						cheackboxs[y][x].isSelected = false;
						cheackboxs[y][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxs[y][x + 1].isSelected || (x == columns - 1 && cheackboxs[y][0].isSelected))
				{
					if (x == columns - 1 && cheackboxs[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						// move the second puyo to left
						cheackboxs[y][x].isFree =		cheackboxs[y][0].isFree;
						cheackboxs[y][x].isSelected =	cheackboxs[y][0].isSelected;
						cheackboxs[y][x].isBase =		cheackboxs[y][0].isBase;
						cheackboxs[y][x].color =		cheackboxs[y][0].color;
						cheackboxs[y][x].sprite =		cheackboxs[y][0].sprite;

						//delete the second puyo 
						cheackboxs[y][0].isFree = true;
						cheackboxs[y][0].isSelected = false;
						cheackboxs[y][0].isBase = false;

						break;
					}
					else if (x > 0 && cheackboxs[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][x - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][x - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][x - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][x - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][x - 1].sprite = cheackboxs[y][x].sprite;

						// move the second puyo to left
						cheackboxs[y][x].isFree = cheackboxs[y][x + 1].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y][x + 1].isBase;
						cheackboxs[y][x].color = cheackboxs[y][x + 1].color;
						cheackboxs[y][x].sprite = cheackboxs[y][x + 1].sprite;

						//delete the second puyo 
						cheackboxs[y][x + 1].isFree = true;
						cheackboxs[y][x + 1].isSelected = false;
						cheackboxs[y][x + 1].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxs[y][columns - 1].isFree)
					{
						// move the puyo base to left
						cheackboxs[y][columns - 1].isFree = cheackboxs[y][x].isFree;
						cheackboxs[y][columns - 1].isSelected = cheackboxs[y][x].isSelected;
						cheackboxs[y][columns - 1].isBase = cheackboxs[y][x].isBase;
						cheackboxs[y][columns - 1].color = cheackboxs[y][x].color;
						cheackboxs[y][columns - 1].sprite = cheackboxs[y][x].sprite;

						// move the second puyo to left
						cheackboxs[y][x].isFree = cheackboxs[y][x + 1].isFree;
						cheackboxs[y][x].isSelected = cheackboxs[y][x + 1].isSelected;
						cheackboxs[y][x].isBase = cheackboxs[y][x + 1].isBase;
						cheackboxs[y][x].color = cheackboxs[y][x + 1].color;
						cheackboxs[y][x].sprite = cheackboxs[y][x + 1].sprite;

						//delete the second puyo 
						cheackboxs[y][x + 1].isFree = true;
						cheackboxs[y][x + 1].isSelected = false;
						cheackboxs[y][x + 1].isBase = false;

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

	if (changeDown)
	{
		for (int y = 1; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				if (!cheackboxs[y][x].isFree && cheackboxs[y - 1][x].isFree && cheackboxs[y][x].color != 6)
				{
					cheackboxs[y - 1][x].isFree = cheackboxs[y][x].isFree;
					cheackboxs[y - 1][x].isSelected = cheackboxs[y][x].isSelected;
					cheackboxs[y - 1][x].isBase = cheackboxs[y][x].isBase;
					cheackboxs[y - 1][x].color = cheackboxs[y][x].color;
					cheackboxs[y - 1][x].sprite = cheackboxs[y][x].sprite;

					cheackboxs[y][x].isFree = true;
					cheackboxs[y][x].isSelected = false;
					cheackboxs[y][x].isBase = false;

					success = true;

				}
			}
		}
	}
	else
	{
		for (int y = rows - 2; y > -1; y--)
		{
			for (int x = columns - 1; x > -1; x--)
			{
				if (!cheackboxs[y][x].isFree && cheackboxs[y + 1][x].isFree && cheackboxs[y][x].color != 6)
				{
					cheackboxs[y + 1][x].isFree = cheackboxs[y][x].isFree;
					cheackboxs[y + 1][x].isSelected = cheackboxs[y][x].isSelected;
					cheackboxs[y + 1][x].isBase = cheackboxs[y][x].isBase;
					cheackboxs[y + 1][x].color = cheackboxs[y][x].color;
					cheackboxs[y + 1][x].sprite = cheackboxs[y][x].sprite;

					cheackboxs[y][x].isFree = true;
					cheackboxs[y][x].isSelected = false;
					cheackboxs[y][x].isBase = false;

					success = true;

				}
			}
		}
	}

	return success;
}

void board::newPuyos()
{
	//unselect puyos
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (cheackboxs[y][x].isSelected)
			{
				cheackboxs[y][x].isSelected = false;
				cheackboxs[y][x].isBase = false;
			}
		}
	}

	int positionX = columns / 2;
	int firstPuyoPositionY = 0;
	int secondPuyoPositionY = 0;

	if (changeDown)
	{
		firstPuyoPositionY = rows -2;
		secondPuyoPositionY = rows -1;
	}
	else
	{
		firstPuyoPositionY = 1;
		secondPuyoPositionY = 0;
	}

	cheackboxs[firstPuyoPositionY][positionX].isSelected = true;
	cheackboxs[firstPuyoPositionY][positionX].isFree = false;
	cheackboxs[firstPuyoPositionY][positionX].isBase = true;
	cheackboxs[firstPuyoPositionY][positionX].sprite = 0;
	int randomColor = (rand() % 6);
	cheackboxs[firstPuyoPositionY][positionX].color = randomColor;

	cheackboxs[secondPuyoPositionY][positionX].isSelected = true;
	cheackboxs[secondPuyoPositionY][positionX].isFree = false;
	cheackboxs[secondPuyoPositionY][positionX].isBase = false;
	cheackboxs[secondPuyoPositionY][positionX].sprite = 0;
	randomColor = (rand() % 6);
	cheackboxs[secondPuyoPositionY][positionX].color = randomColor;
}

bool board::endGame()
{
	bool endGame = false;

	int positionX = columns / 2;
	
	if (changeDown)
	{
		int positionY = rows - 2;

		if (cheackboxs[positionY][positionX].isBase && !cheackboxs[positionY - 1][positionX].isFree && !cheackboxs[positionY - 2][positionX].isFree)
		{
			endGame = true;
		}
	}
	else
	{
		int positionY = 1;

		if (cheackboxs[positionY][positionX].isBase && !cheackboxs[positionY + 1][positionX].isFree && !cheackboxs[positionY + 2][positionX].isFree)
		{
			endGame = true;
		}
	}

	return endGame;
}

bool board::updateSprite()
{
	bool success = false;

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
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
				if (!cheackboxs[y + 1][x].isFree && cheackboxs[y + 1][x].color == color && y != rows - 1)
				{
					down = true;
				}

				//check left
				if (!cheackboxs[y][x - 1].isFree && cheackboxs[y][x - 1].color == color && x != 0)
				{
					left = true;
				}

				//check right
				if (!cheackboxs[y][x + 1].isFree && cheackboxs[y][x + 1].color == color && x != columns -1)
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

	for (int y = rows-1; y > -1; y--)
	{
		for (int x = columns -1; x > -1; x--)
		{
			if (!cheackboxs[y][x].isFree)
			{
				//delete array with value to delete puyos
				for (int i = 0; i < 10; i++)
				{
					deletesPuyos[i].x = 0;
					deletesPuyos[i].y = 0;
					deletesPuyos[i].empty = true;
					deletesPuyos[i].visited = false;
				}

				deletesPuyos[0].x = x;
				deletesPuyos[0].y = y;
				deletesPuyos[0].empty = false;
				deletesPuyos[0].visited = true;

				deletePuyos(y, x);

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
						}
					}

					updateSprite();

					moveDown();

					combos = combos + 1;
					
					system("CLS");
					printf("\n Hello Welcome to Puyo Puyo \n\n");
					printf("Combos: %d \n\n", combos);

					timeDelay = timeDelay - 40;
					success = true;
				}
			}
		}
	}

	if (success)
	{
		//flipBoard();
	}

	return success;
}

void board::deletePuyos(int y, int x)
{
	int color = cheackboxs[y][x].color;

	//up
	if (!cheackboxs[y - 1][x].isFree && cheackboxs[y - 1][x].color == color && y != 0 && !isVisited(x, y - 1))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x;
				deletesPuyos[i].y = y - 1;
				deletesPuyos[i].empty = false;
				deletesPuyos[i].visited = true;
				break;
			}
		}

		deletePuyos(y - 1, x);
	}

	//down
	if (!cheackboxs[y + 1][x].isFree && cheackboxs[y + 1][x].color == color && y != rows -1 && !isVisited(x, y + 1))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x;
				deletesPuyos[i].y = y + 1;
				deletesPuyos[i].empty = false;
				deletesPuyos[i].visited = true;
				break;
			}
		}

		deletePuyos(y + 1, x);
	}

	//left
	if (!cheackboxs[y][x - 1].isFree && cheackboxs[y][x - 1].color == color && x != 0 && !isVisited(x - 1, y))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x - 1;
				deletesPuyos[i].y = y;
				deletesPuyos[i].empty = false;
				deletesPuyos[i].visited = true;
				break;
			}
		}

		deletePuyos(y, x - 1);
	}

	//right
	if (!cheackboxs[y][x + 1].isFree && cheackboxs[y][x + 1].color == color && x != columns -1 && !isVisited(x + 1, y))
	{
		for (int i = 0; i < 10; i++)
		{
			if (deletesPuyos[i].empty)
			{
				deletesPuyos[i].x = x + 1;
				deletesPuyos[i].y = y;
				deletesPuyos[i].empty = false;
				deletesPuyos[i].visited = true;
				break;
			}
		}

		deletePuyos(y, x + 1);
	}
}

bool board::isVisited(int newX, int newY)
{
	bool success = false;

	for (int i = 0; i < 10; i++)
	{
		if (!deletesPuyos[i].empty)
		{
			if (deletesPuyos[i].x == newX && deletesPuyos[i].y == newY)
			{
				success = true;
			}
		}
	}

	return success;
}

void board::flipBoard()
{
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			cheackboxsAux[rows - 1 - y][columns - 1 - x].color = cheackboxs[y][x].color;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isBase = cheackboxs[y][x].isBase;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isFree = cheackboxs[y][x].isFree;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isSelected = cheackboxs[y][x].isSelected;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].sprite = cheackboxs[y][x].sprite;
		}
	}

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//cheackboxs[y][x] = cheackboxsAux[y][x];

			cheackboxs[y][x].color = cheackboxsAux[y][x].color;
			cheackboxs[y][x].isBase = cheackboxsAux[y][x].isBase;
			cheackboxs[y][x].isFree = cheackboxsAux[y][x].isFree;
			cheackboxs[y][x].isSelected = cheackboxsAux[y][x].isSelected;
			cheackboxs[y][x].sprite = cheackboxsAux[y][x].sprite;
		}
	}

	changeDown = !changeDown;

	updateSprite();

}

void board::changePuyo()
{
	int colorPuyoBase;
	int colorPuyo;
	
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (cheackboxs[y][x].isBase)
			{
				colorPuyoBase = cheackboxs[y][x].color;
			}
			else if (cheackboxs[y][x].isSelected)
			{
				colorPuyo = cheackboxs[y][x].color;
			}
		}
	}

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (cheackboxs[y][x].isBase)
			{
				cheackboxs[y][x].color = colorPuyo;
			}
			else if (cheackboxs[y][x].isSelected)
			{
				cheackboxs[y][x].color = colorPuyoBase;
			}
		}
	}

	//updateSprite();

}
