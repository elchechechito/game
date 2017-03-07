#include "board.h"
#include <time.h>
#include <random>

board::board()
{
	m_timestamp = 0;
	m_timestampPC = 0;

	changeDown = false;

	timeDelay = 500;
	timeDelayPC = 500;
	combos = 0;
	combosPC = 0;

	rows = 16;
	columns = 11;

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//board 1
			cheackboxs[y][x].box.x = 40 + (33 * x);
			cheackboxs[y][x].box.y = 40 + (31 * y);
			cheackboxs[y][x].box.w = 33;
			cheackboxs[y][x].box.h = 31;

			cheackboxs[y][x].isFree = true;
			cheackboxs[y][x].isSelected = false;
			cheackboxs[y][x].isBase = false;
			cheackboxs[y][x].color = 10;
			cheackboxs[y][x].isGhost = false;
			cheackboxs[y][x].auxPositionColor = 10;

			//board 2
			cheackboxs2[y][x].box.x = 450 + (33 * x);
			cheackboxs2[y][x].box.y = 40 + (31 * y);
			cheackboxs2[y][x].box.w = 33;
			cheackboxs2[y][x].box.h = 31;

			cheackboxs2[y][x].isFree = true;
			cheackboxs2[y][x].isSelected = false;
			cheackboxs2[y][x].isBase = false;
			cheackboxs2[y][x].color = 10;
			cheackboxs[y][x].isGhost = false;
		}
	}

	//add puyo ghost
	cheackboxs[rows - 1][columns - 1].isGhost = true;	
	cheackboxs[rows - 1][columns - 1].auxPositionColor = 10;
	
	srand(time(0));

	for (int i = 0; i < 3; i++)
	{
		int randomX = rand() % columns;
		int randomY = rand() % rows;

		//board 1
		cheackboxs[randomY][randomX].isFree = false;
		cheackboxs[randomY][randomX].isSelected = false;
		cheackboxs[randomY][randomX].isBase = false;
		cheackboxs[randomY][randomX].color = 6;
		
		//board 2
		cheackboxs2[randomY][randomX].isFree = false;
		cheackboxs2[randomY][randomX].isSelected = false;
		cheackboxs2[randomY][randomX].isBase = false;
		cheackboxs2[randomY][randomX].color = 6;

	}
}

board::~board()
{
}

void board::startGameBoard()
{
	//time start
	m_timestamp = SDL_GetTicks();
	m_timestampPC = SDL_GetTicks();

	int gameTime = 0;
	int gameTime_stamp = 0;

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
						if (pause)
						{
							gameTime_stamp = SDL_GetTicks();
						}

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

				if (!pause)
				{
					int time = SDL_GetTicks() - gameTime_stamp;
					if (time > 1000)
					{
						gameTime_stamp = SDL_GetTicks();

						gameTime = gameTime + time;

						//Show welcome message and combos
						system("CLS");
						printf("\n Hello Welcome to Puyo Puyo\n\n");
						printf(" Time: %d Seconds\n\n", gameTime / 1000);
						printf(" Combos: %d", combos);
						printf("\n Combos CPU: %d\n\n", combosPC);
					}
				}
				
				// - Player
				int elapsed_time = SDL_GetTicks() - m_timestamp; 
				if (elapsed_time > timeDelay)
				{
					if (!pause)
					{
						m_timestamp = SDL_GetTicks();
						
						// - Player -
						
						//move down or up all piece
						bool moved = moveDown(true);

						puyoGhostAttack();

						if (!moved)
						{
							//Update all sprite
							bool isUpdate = updateSprite(true);

							if (!isUpdate)
							{
								//Cheack if the puyos were removed
								bool isDeletePuyos = checkDeletePuyo();

								if (!isDeletePuyos)
								{
									newPuyos(true);
								}
							}
						}						
					}
				}
				
				// - PC
				int elapsed_timePC = SDL_GetTicks() - m_timestampPC;
				if (elapsed_timePC > timeDelayPC)
				{
					if (!pause)
					{
						m_timestampPC = SDL_GetTicks();

						// - PC -

						int randomMovement = rand() % 8;
						bool ismoveDown = true;

						switch (randomMovement)
						{
							//turn
						case 0:
							turnPuyo(false);
							break;

							//down
						case 1:
							/*while (ismoveDown)
							{
								ismoveDown = moveDownPC();
							}
							break;*/

							//left
						case 2:
							moveLeft(false);
							break;

							//right
						case 3:
							moveRight(false);
							break;
						}

						//move down or up all piece PC
						bool moved = moveDown(false);

						if (!moved)
						{
							//Update all sprite
							bool isUpdate = updateSprite(false);

							if (!isUpdate)
							{
								//Cheack if the puyos were removed
								bool isDeletePuyos = checkDeletePuyoPC();

								if (!isDeletePuyos)
								{
									newPuyos(false);
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
	fillRect = { 450, 40, columns * 33, rows * 31 };
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

				case 7:
					puyoClip = &sdl.ghostPuyoSprite;
					break;

				}

				sdl.render(cheackboxs[y][x].box.x, cheackboxs[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
			}

			if (cheackboxs[y][x].isGhost)
			{
				SDL_Rect* puyoClip = NULL;
				puyoClip = &sdl.ghostPuyoSprite;
				sdl.render(cheackboxs[y][x].box.x, cheackboxs[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
			}

			if (cheackboxs2[y][x].isFree == false)
			{
				SDL_Rect* puyoClip = NULL;
				int sprite = cheackboxs2[y][x].sprite;

				switch (cheackboxs2[y][x].color)
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

				sdl.render(cheackboxs2[y][x].box.x, cheackboxs2[y][x].box.y, sdl.gTexturePuyo, puyoClip, false);
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
			turnPuyo(true);
			break;

		case SDLK_RIGHT:
			moveRight(true);
			break;

		case SDLK_LEFT:
			moveLeft(true);
			break;

		case SDLK_SPACE:
			changePuyo();
			break;

		case SDLK_DOWN:
			bool isMoveDown = true;

			while (isMoveDown)
			{
				isMoveDown = moveDown(true);
			}

			break;
		}
	}
}

void board::turnPuyo(bool player = true)
{
	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	//turn puyo
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//search the puyo base
			if (cheackboxsAux[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxsAux[y - 1][x].isSelected)
				{
					//if the puyo base is in the last box on the right
					if (x == columns - 1 && cheackboxsAux[y][x - 1].isFree)
					{
						//move puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						//move up puyo to down
						cheackboxsAux[y][x].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete up puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;
						cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][x].sprite;

						break;

					}
					else if (cheackboxsAux[y][x + 1].isFree)
					{
						// up puyo to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete up puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;
						cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][x].sprite;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxsAux[y][x + 1].isSelected)
				{
					if (cheackboxsAux[y + 1][x].isFree && y != rows - 1)
					{
						// right puyo to dow
						cheackboxsAux[y + 1][x].isFree = cheackboxsAux[y][x + 1].isFree;
						cheackboxsAux[y + 1][x].isSelected = cheackboxsAux[y][x + 1].isSelected;
						cheackboxsAux[y + 1][x].isBase = cheackboxsAux[y][x + 1].isBase;
						cheackboxsAux[y + 1][x].color = cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y + 1][x].sprite = cheackboxsAux[y][x + 1].sprite;

						//delete up puyo
						cheackboxsAux[y][x + 1].isFree = true;
						cheackboxsAux[y][x + 1].isSelected = false;
						cheackboxsAux[y][x + 1].isBase = false;
						cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y][x + 1].sprite;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxsAux[y + 1][x].isSelected)
				{
					//if the puyo base is in the first box on the left
					if (x == 0 && cheackboxsAux[y][x + 1].isFree)
					{
						//move puyo base to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						//move down puyo to up
						cheackboxsAux[y][x].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete up puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;
						cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][x].sprite;

						break;

					}
					else if (cheackboxsAux[y][x - 1].isFree)
					{
						// move down puyo to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete up puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;
						cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][x].sprite;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxsAux[y][x - 1].isSelected)
				{
					if (cheackboxsAux[y - 1][x].isFree)
					{
						// left puyo to up
						cheackboxsAux[y - 1][x].isFree = cheackboxsAux[y][x - 1].isFree;
						cheackboxsAux[y - 1][x].isSelected = cheackboxsAux[y][x - 1].isSelected;
						cheackboxsAux[y - 1][x].isBase = cheackboxsAux[y][x - 1].isBase;
						cheackboxsAux[y - 1][x].color = cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y - 1][x].sprite = cheackboxsAux[y][x - 1].sprite;

						//delete up puyo
						cheackboxsAux[y][x - 1].isFree = true;
						cheackboxsAux[y][x - 1].isSelected = false;
						cheackboxsAux[y][x - 1].isBase = false;
						cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y][x - 1].sprite;

						break;
					}

				}

			}
		}
	}

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
			}
		}
	}

}

void board::moveRight(bool player = true)
{
	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	//Move right
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//find the puyo base
			if (cheackboxsAux[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxsAux[y - 1][x].isSelected)
				{
					if (x < columns - 1 && cheackboxsAux[y][x + 1].isFree && cheackboxsAux[y - 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y - 1][x + 1].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y - 1][x + 1].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y - 1][x + 1].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y - 1][x + 1].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][x + 1].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;

						break;
					}
					else if (x == columns - 1 && cheackboxsAux[y][0].isFree && cheackboxsAux[y - 1][0].isFree)
					{
						// move the puyo base to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y - 1][0].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y - 1][0].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y - 1][0].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y - 1][0].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][0].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxsAux[y + 1][x].isSelected)
				{
					if (x < columns - 1 && cheackboxsAux[y][x + 1].isFree && cheackboxsAux[y + 1][x + 1].isFree)
					{
						// move the puyo base to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y + 1][x + 1].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y + 1][x + 1].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y + 1][x + 1].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y + 1][x + 1].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][x + 1].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;

						break;
					}
					else if (x == columns - 1 && cheackboxsAux[y][0].isFree && cheackboxsAux[y + 1][0].isFree)
					{
						// move the puyo base to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y + 1][0].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y + 1][0].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y + 1][0].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y + 1][0].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][0].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxsAux[y][x - 1].isSelected || (x == 0 && cheackboxsAux[y][columns - 1].isSelected))
				{
					if (x == 0 && cheackboxsAux[y][x + 1].isFree)
					{
						// move the puyo bas to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						// move the second puyo to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][columns - 1].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][columns - 1].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][columns - 1].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][columns - 1].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][columns - 1].sprite;

						//delete second puyo
						cheackboxsAux[y][columns - 1].isFree = true;
						cheackboxsAux[y][columns - 1].isSelected = false;
						cheackboxsAux[y][columns - 1].isBase = false;

						break;
					}
					else if (x < columns - 1 && cheackboxsAux[y][x + 1].isFree)
					{
						// move the second puyo to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						// move the puyo base to right
						cheackboxsAux[y][x].isFree = cheackboxsAux[y][x - 1].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y][x - 1].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y][x - 1].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y][x - 1].sprite;

						//delete second puyo
						cheackboxsAux[y][x - 1].isFree = true;
						cheackboxsAux[y][x - 1].isSelected = false;
						cheackboxsAux[y][x - 1].isBase = false;

						break;
					}
					else if (x == columns - 1 && cheackboxsAux[y][0].isFree)
					{
						// move the puyo base to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][x].sprite;

						// move the second puyo to right
						cheackboxsAux[y][x].isFree = cheackboxsAux[y][x - 1].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y][x - 1].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y][x - 1].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y][x - 1].sprite;

						//delete second puyo
						cheackboxsAux[y][x - 1].isFree = true;
						cheackboxsAux[y][x - 1].isSelected = false;
						cheackboxsAux[y][x - 1].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxsAux[y][x + 1].isSelected || (x == columns - 1 && cheackboxsAux[y][0].isSelected))
				{
					if (x < columns - 2 && cheackboxsAux[y][x + 2].isFree)
					{
						// move the second puyo to right
						cheackboxsAux[y][x + 2].isFree = cheackboxsAux[y][x + 1].isFree;
						cheackboxsAux[y][x + 2].isSelected = cheackboxsAux[y][x + 1].isSelected;
						cheackboxsAux[y][x + 2].isBase = cheackboxsAux[y][x + 1].isBase;
						cheackboxsAux[y][x + 2].color = cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y][x + 2].sprite = cheackboxsAux[y][x + 1].sprite;

						// move the puyo base to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
					else if (x == columns - 2 && cheackboxsAux[y][0].isFree)
					{
						// move the second puyo to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][x + 1].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][x + 1].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][x + 1].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][x + 1].sprite;

						// move the puyo base to right
						cheackboxsAux[y][x + 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x + 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x + 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x + 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x + 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
					else if (x == columns - 1 && cheackboxsAux[y][1].isFree)
					{
						// move the second puyo to right
						cheackboxsAux[y][1].isFree = cheackboxsAux[y][0].isFree;
						cheackboxsAux[y][1].isSelected = cheackboxsAux[y][0].isSelected;
						cheackboxsAux[y][1].isBase = cheackboxsAux[y][0].isBase;
						cheackboxsAux[y][1].color = cheackboxsAux[y][0].color;
						cheackboxsAux[y][1].sprite = cheackboxsAux[y][0].sprite;

						// move the puyo base to right
						cheackboxsAux[y][0].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][0].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][0].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][0].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][0].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
				}
			}
		}
	}

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
			}
		}
	}
}

void board::moveLeft(bool player = true)
{
	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	//move left
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			//find the puyo base
			if (cheackboxsAux[y][x].isBase)
			{
				//if the second puyo is up
				if (cheackboxsAux[y - 1][x].isSelected)
				{
					if (x > 0 && cheackboxsAux[y][x - 1].isFree && cheackboxsAux[y - 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to left
						cheackboxsAux[y - 1][x - 1].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y - 1][x - 1].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y - 1][x - 1].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y - 1][x - 1].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][x - 1].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxsAux[y][columns - 1].isFree && cheackboxsAux[y - 1][columns - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][columns - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][columns - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][columns - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][columns - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][columns - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to left
						cheackboxsAux[y - 1][columns - 1].isFree = cheackboxsAux[y - 1][x].isFree;
						cheackboxsAux[y - 1][columns - 1].isSelected = cheackboxsAux[y - 1][x].isSelected;
						cheackboxsAux[y - 1][columns - 1].isBase = cheackboxsAux[y - 1][x].isBase;
						cheackboxsAux[y - 1][columns - 1].color = cheackboxsAux[y - 1][x].color;
						cheackboxsAux[y - 1][columns - 1].sprite = cheackboxsAux[y - 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y - 1][x].isFree = true;
						cheackboxsAux[y - 1][x].isSelected = false;
						cheackboxsAux[y - 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is down
				else if (cheackboxsAux[y + 1][x].isSelected)
				{
					if (x > 0 && cheackboxsAux[y][x - 1].isFree && cheackboxsAux[y + 1][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y + 1][x - 1].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y + 1][x - 1].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y + 1][x - 1].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y + 1][x - 1].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][x - 1].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxsAux[y][columns - 1].isFree && cheackboxsAux[y + 1][columns - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][columns - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][columns - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][columns - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][columns - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][columns - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete puyo base
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						// move the second puyo to right
						cheackboxsAux[y + 1][columns - 1].isFree = cheackboxsAux[y + 1][x].isFree;
						cheackboxsAux[y + 1][columns - 1].isSelected = cheackboxsAux[y + 1][x].isSelected;
						cheackboxsAux[y + 1][columns - 1].isBase = cheackboxsAux[y + 1][x].isBase;
						cheackboxsAux[y + 1][columns - 1].color = cheackboxsAux[y + 1][x].color;
						cheackboxsAux[y + 1][columns - 1].sprite = cheackboxsAux[y + 1][x].sprite;

						//delete second puyo
						cheackboxsAux[y + 1][x].isFree = true;
						cheackboxsAux[y + 1][x].isSelected = false;
						cheackboxsAux[y + 1][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the left
				else if (cheackboxsAux[y][x - 1].isSelected || (x == 0 && cheackboxsAux[y][columns - 1].isSelected))
				{
					if (x > 1 && cheackboxsAux[y][x - 2].isFree)
					{
						// move the second puyo to left
						cheackboxsAux[y][x - 2].isFree = cheackboxsAux[y][x - 1].isFree;
						cheackboxsAux[y][x - 2].isSelected = cheackboxsAux[y][x - 1].isSelected;
						cheackboxsAux[y][x - 2].isBase = cheackboxsAux[y][x - 1].isBase;
						cheackboxsAux[y][x - 2].color = cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y][x - 2].sprite = cheackboxsAux[y][x - 1].sprite;

						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete second puyo
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
					else if (x == 1 && cheackboxsAux[y][columns - 1].isFree)
					{
						// move the second puyo to left
						cheackboxsAux[y][columns - 1].isFree = cheackboxsAux[y][x - 1].isFree;
						cheackboxsAux[y][columns - 1].isSelected = cheackboxsAux[y][x - 1].isSelected;
						cheackboxsAux[y][columns - 1].isBase = cheackboxsAux[y][x - 1].isBase;
						cheackboxsAux[y][columns - 1].color = cheackboxsAux[y][x - 1].color;
						cheackboxsAux[y][columns - 1].sprite = cheackboxsAux[y][x - 1].sprite;

						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete second puyo
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxsAux[y][columns - 2].isFree)
					{
						// move the second puyo to left
						cheackboxsAux[y][columns - 2].isFree = cheackboxsAux[y][columns - 1].isFree;
						cheackboxsAux[y][columns - 2].isSelected = cheackboxsAux[y][columns - 1].isSelected;
						cheackboxsAux[y][columns - 2].isBase = cheackboxsAux[y][columns - 1].isBase;
						cheackboxsAux[y][columns - 2].color = cheackboxsAux[y][columns - 1].color;
						cheackboxsAux[y][columns - 2].sprite = cheackboxsAux[y][columns - 1].sprite;

						// move the puyo base to left
						cheackboxsAux[y][columns - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][columns - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][columns - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][columns - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][columns - 1].sprite = cheackboxsAux[y][x].sprite;

						//delete second puyo
						cheackboxsAux[y][x].isFree = true;
						cheackboxsAux[y][x].isSelected = false;
						cheackboxsAux[y][x].isBase = false;

						break;
					}
				}

				//if the second puyo is on the right
				else if (cheackboxsAux[y][x + 1].isSelected || (x == columns - 1 && cheackboxsAux[y][0].isSelected))
				{
					if (x == columns - 1 && cheackboxsAux[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						// move the second puyo to left
						cheackboxsAux[y][x].isFree = cheackboxsAux[y][0].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y][0].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y][0].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y][0].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y][0].sprite;

						//delete the second puyo 
						cheackboxsAux[y][0].isFree = true;
						cheackboxsAux[y][0].isSelected = false;
						cheackboxsAux[y][0].isBase = false;

						break;
					}
					else if (x > 0 && cheackboxsAux[y][x - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][x - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][x - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][x - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][x - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][x - 1].sprite = cheackboxsAux[y][x].sprite;

						// move the second puyo to left
						cheackboxsAux[y][x].isFree = cheackboxsAux[y][x + 1].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y][x + 1].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y][x + 1].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y][x + 1].sprite;

						//delete the second puyo 
						cheackboxsAux[y][x + 1].isFree = true;
						cheackboxsAux[y][x + 1].isSelected = false;
						cheackboxsAux[y][x + 1].isBase = false;

						break;
					}
					else if (x == 0 && cheackboxsAux[y][columns - 1].isFree)
					{
						// move the puyo base to left
						cheackboxsAux[y][columns - 1].isFree = cheackboxsAux[y][x].isFree;
						cheackboxsAux[y][columns - 1].isSelected = cheackboxsAux[y][x].isSelected;
						cheackboxsAux[y][columns - 1].isBase = cheackboxsAux[y][x].isBase;
						cheackboxsAux[y][columns - 1].color = cheackboxsAux[y][x].color;
						cheackboxsAux[y][columns - 1].sprite = cheackboxsAux[y][x].sprite;

						// move the second puyo to left
						cheackboxsAux[y][x].isFree = cheackboxsAux[y][x + 1].isFree;
						cheackboxsAux[y][x].isSelected = cheackboxsAux[y][x + 1].isSelected;
						cheackboxsAux[y][x].isBase = cheackboxsAux[y][x + 1].isBase;
						cheackboxsAux[y][x].color = cheackboxsAux[y][x + 1].color;
						cheackboxsAux[y][x].sprite = cheackboxsAux[y][x + 1].sprite;

						//delete the second puyo 
						cheackboxsAux[y][x + 1].isFree = true;
						cheackboxsAux[y][x + 1].isSelected = false;
						cheackboxsAux[y][x + 1].isBase = false;

						break;
					}
				}

			}
		}
	}

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
			}
		}
	}
}

bool board::moveDown(bool player = true)
{
	bool success = false;

	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	if (changeDown)
	{
		for (int y = 1; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				if (!cheackboxsAux[y][x].isFree && cheackboxsAux[y - 1][x].isFree && cheackboxsAux[y][x].color != 6)
				{
					cheackboxsAux[y - 1][x].isFree = cheackboxsAux[y][x].isFree;
					cheackboxsAux[y - 1][x].isSelected = cheackboxsAux[y][x].isSelected;
					cheackboxsAux[y - 1][x].isBase = cheackboxsAux[y][x].isBase;
					cheackboxsAux[y - 1][x].color = cheackboxsAux[y][x].color;
					cheackboxsAux[y - 1][x].sprite = cheackboxsAux[y][x].sprite;

					cheackboxsAux[y][x].isFree = true;
					cheackboxsAux[y][x].isSelected = false;
					cheackboxsAux[y][x].isBase = false;

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
				if (!cheackboxsAux[y][x].isFree && cheackboxsAux[y + 1][x].isFree && cheackboxsAux[y][x].color != 6)
				{
					cheackboxsAux[y + 1][x].isFree = cheackboxsAux[y][x].isFree;
					cheackboxsAux[y + 1][x].isSelected = cheackboxsAux[y][x].isSelected;
					cheackboxsAux[y + 1][x].isBase = cheackboxsAux[y][x].isBase;
					cheackboxsAux[y + 1][x].color = cheackboxsAux[y][x].color;
					cheackboxsAux[y + 1][x].sprite = cheackboxsAux[y][x].sprite;

					cheackboxsAux[y][x].isFree = true;
					cheackboxsAux[y][x].isSelected = false;
					cheackboxsAux[y][x].isBase = false;

					success = true;

				}
			}
		}
	}

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
			}
		}
	}

	return success;
}

void board::newPuyos(bool player)
{
	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	//unselect puyos
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (cheackboxsAux[y][x].isSelected)
			{
				cheackboxsAux[y][x].isSelected = false;
				cheackboxsAux[y][x].isBase = false;
			}
		}
	}

	int positionX = columns / 2;
	int firstPuyoPositionY = 0;
	int secondPuyoPositionY = 0;

	if (changeDown)
	{
		firstPuyoPositionY = rows - 2;
		secondPuyoPositionY = rows - 1;
	}
	else
	{
		firstPuyoPositionY = 1;
		secondPuyoPositionY = 0;
	}

	//first board
	cheackboxsAux[firstPuyoPositionY][positionX].isSelected = true;
	cheackboxsAux[firstPuyoPositionY][positionX].isFree = false;
	cheackboxsAux[firstPuyoPositionY][positionX].isBase = true;
	cheackboxsAux[firstPuyoPositionY][positionX].sprite = 0;
	int randomColor = (rand() % 6);
	cheackboxsAux[firstPuyoPositionY][positionX].color = randomColor;

	cheackboxsAux[secondPuyoPositionY][positionX].isSelected = true;
	cheackboxsAux[secondPuyoPositionY][positionX].isFree = false;
	cheackboxsAux[secondPuyoPositionY][positionX].isBase = false;
	cheackboxsAux[secondPuyoPositionY][positionX].sprite = 0;
	randomColor = (rand() % 6);
	cheackboxsAux[secondPuyoPositionY][positionX].color = randomColor;

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
			}
		}
	}

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

bool board::updateSprite(bool player = true)
{
	bool success = false;

	//Copy to cheackboxs aux
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxsAux[y][x] = cheackboxs[y][x];
			}
			else
			{
				cheackboxsAux[y][x] = cheackboxs2[y][x];
			}
		}
	}

	//Update Sprites
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (!cheackboxsAux[y][x].isFree)
			{
				int color = cheackboxsAux[y][x].color;
				bool up = false, down = false, left = false, right = false;

				// check up 
				if (!cheackboxsAux[y - 1][x].isFree && cheackboxsAux[y - 1][x].color == color)
				{
					up = true;
				}

				//check down
				if (!cheackboxsAux[y + 1][x].isFree && cheackboxsAux[y + 1][x].color == color && y != rows - 1)
				{
					down = true;
				}

				//check left
				if (!cheackboxsAux[y][x - 1].isFree && cheackboxsAux[y][x - 1].color == color && x != 0)
				{
					left = true;
				}

				//check right
				if (!cheackboxsAux[y][x + 1].isFree && cheackboxsAux[y][x + 1].color == color && x != columns - 1)
				{
					right = true;
				}

				if (up && down && left && right)
				{
					if (cheackboxsAux[y][x].sprite != 15)
					{
						cheackboxsAux[y][x].sprite = 15;
						success = true;
					}
				}
				else if (up && left && right)
				{
					if (cheackboxsAux[y][x].sprite != 14)
					{
						cheackboxsAux[y][x].sprite = 14;
						success = true;
					}
				}
				else if (down && left && right)
				{
					if (cheackboxsAux[y][x].sprite != 13)
					{
						cheackboxsAux[y][x].sprite = 13;
						success = true;
					}
				}
				else if (up && down && left)
				{
					if (cheackboxsAux[y][x].sprite != 11)
					{
						cheackboxsAux[y][x].sprite = 11;
						success = true;
					}
				}
				else if (up && down && right)
				{
					if (cheackboxsAux[y][x].sprite != 7)
					{
						cheackboxsAux[y][x].sprite = 7;
						success = true;
					}
				}
				else if (up && down)
				{
					if (cheackboxsAux[y][x].sprite != 3)
					{
						cheackboxsAux[y][x].sprite = 3;
						success = true;
					}
				}
				else if (up && left)
				{
					if (cheackboxsAux[y][x].sprite != 10)
					{
						cheackboxsAux[y][x].sprite = 10;
						success = true;
					}
				}
				else if (up && right)
				{
					if (cheackboxsAux[y][x].sprite != 6)
					{
						cheackboxsAux[y][x].sprite = 6;
						success = true;
					}
				}
				else if (down && left)
				{
					if (cheackboxsAux[y][x].sprite != 9)
					{
						cheackboxsAux[y][x].sprite = 9;
						success = true;
					}
				}
				else if (down && right)
				{
					if (cheackboxsAux[y][x].sprite != 5)
					{
						cheackboxsAux[y][x].sprite = 5;
						success = true;
					}
				}
				else if (left && right)
				{
					if (cheackboxsAux[y][x].sprite != 12)
					{
						cheackboxsAux[y][x].sprite = 12;
						success = true;
					}
				}
				else if (up)
				{
					if (cheackboxsAux[y][x].sprite != 2)
					{
						cheackboxsAux[y][x].sprite = 2;
						success = true;
					}
				}
				else if (down)
				{
					if (cheackboxsAux[y][x].sprite != 1)
					{
						cheackboxsAux[y][x].sprite = 1;
						success = true;
					}
				}
				else if (left)
				{
					if (cheackboxsAux[y][x].sprite != 8)
					{
						cheackboxsAux[y][x].sprite = 8;
						success = true;
					}
				}
				else if (right)
				{
					if (cheackboxsAux[y][x].sprite != 4)
					{
						cheackboxsAux[y][x].sprite = 4;
						success = true;
					}
				}
				else
				{
					if (cheackboxsAux[y][x].sprite != 0)
					{
						cheackboxsAux[y][x].sprite = 0;
						success = true;
					}
				}
			}
		}
	}

	//copy to cheackboxs
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (player)
			{
				cheackboxs[y][x] = cheackboxsAux[y][x];
			}
			else
			{
				cheackboxs2[y][x] = cheackboxsAux[y][x];
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

					combos = combos + 1;
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

bool board::checkDeletePuyoPC()
{
	bool success = false;

	for (int y = rows - 1; y > -1; y--)
	{
		for (int x = columns - 1; x > -1; x--)
		{
			if (!cheackboxs2[y][x].isFree)
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

				deletePuyosPC(y, x);

				if (!deletesPuyos[3].empty)
				{
					//delete puyos
					for (int i = 0; i < 10; i++)
					{
						if (!deletesPuyos[i].empty)
						{
							cheackboxs2[deletesPuyos[i].y][deletesPuyos[i].x].isFree = true;
							cheackboxs2[deletesPuyos[i].y][deletesPuyos[i].x].isBase = false;
							cheackboxs2[deletesPuyos[i].y][deletesPuyos[i].x].isSelected = false;
						}
					}

					combosPC = combosPC + 1;
					timeDelayPC = timeDelayPC - 40;
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

void board::deletePuyosPC(int y, int x)
{
	int color = cheackboxs2[y][x].color;

	//up
	if (!cheackboxs2[y - 1][x].isFree && cheackboxs2[y - 1][x].color == color && y != 0 && !isVisited(x, y - 1))
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

		deletePuyosPC(y - 1, x);
	}

	//down
	if (!cheackboxs2[y + 1][x].isFree && cheackboxs2[y + 1][x].color == color && y != rows - 1 && !isVisited(x, y + 1))
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

		deletePuyosPC(y + 1, x);
	}

	//left
	if (!cheackboxs2[y][x - 1].isFree && cheackboxs2[y][x - 1].color == color && x != 0 && !isVisited(x - 1, y))
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

		deletePuyosPC(y, x - 1);
	}

	//right
	if (!cheackboxs2[y][x + 1].isFree && cheackboxs2[y][x + 1].color == color && x != columns - 1 && !isVisited(x + 1, y))
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

		deletePuyosPC(y, x + 1);
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

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			cheackboxsAux[rows - 1 - y][columns - 1 - x].color = cheackboxs2[y][x].color;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isBase = cheackboxs2[y][x].isBase;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isFree = cheackboxs2[y][x].isFree;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].isSelected = cheackboxs2[y][x].isSelected;
			cheackboxsAux[rows - 1 - y][columns - 1 - x].sprite = cheackboxs2[y][x].sprite;
		}
	}

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			cheackboxs2[y][x].color = cheackboxsAux[y][x].color;
			cheackboxs2[y][x].isBase = cheackboxsAux[y][x].isBase;
			cheackboxs2[y][x].isFree = cheackboxsAux[y][x].isFree;
			cheackboxs2[y][x].isSelected = cheackboxsAux[y][x].isSelected;
			cheackboxs2[y][x].sprite = cheackboxsAux[y][x].sprite;
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

void board::puyoGhostAttack()
{
	bool puyoGhostAttack = false;

	for (int y = rows -1; y > -1; y--)
	{
		for (int x = columns -1; x > -1; x--)
		{
			if (cheackboxs[y][x].isGhost)
			{
				//cheack if have puyo above
				if (!cheackboxs[y -1][x].isFree && y != 0)
				{
					cheackboxs[y][x].color = cheackboxs[y -1][x].color;
					
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
						puyoGhostAttack = true;
					}
					else
					{
						cheackboxs[y][x].color = 7;
					}
				}

				//cheack if have puyo down
				else if (!cheackboxs[y +1][x].isFree && y != rows -1)
				{
					cheackboxs[y][x].color = cheackboxs[y +1][x].color;

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
						puyoGhostAttack = true;
					}
					else
					{
						cheackboxs[y][x].color = 7;
					}
				}

				//cheack if have puyo on the left
				else if (!cheackboxs[y][x -1].isFree && x != 0)
				{
					cheackboxs[y][x].color = cheackboxs[y][x -1].color;

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
						puyoGhostAttack = true;
					}
					else
					{
						cheackboxs[y][x].color = 7;
					}
				}

				//cheack if have puyo on the right
				else if (!cheackboxs[y][x +1].isFree && x != columns -1)
				{
					cheackboxs[y][x].color = cheackboxs[y][x +1].color;

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
						puyoGhostAttack = true;
					}
					else
					{
						cheackboxs[y][x].color = 7;
					}
				}
			
				//move puyoGhost
				if (puyoGhostAttack)
				{
					if (x == 0)
					{
						if (y == 0)
						{
							cheackboxs[y][x].isGhost = false;
							cheackboxs[rows - 1][columns - 1].isGhost = true;
							cheackboxs[rows - 1][columns - 1].auxPositionColor = cheackboxs[rows -1][columns - 1].color;
						}
						else
						{
							cheackboxs[y][x].isGhost = false;
							cheackboxs[y - 1][columns - 1].isGhost = true;
							cheackboxs[y - 1][columns - 1].auxPositionColor = cheackboxs[y -1][columns - 1].color;
						}
					}
					else
					{
						cheackboxs[y][x].isGhost = false;
						cheackboxs[y][x - 1].isGhost = true;
						cheackboxs[y][x - 1].auxPositionColor = cheackboxs[y][x - 1].color;						
					}
					
				}
				else
				{
					if (x == 0)
					{
						if (y == 0)
						{
							cheackboxs[y][x].isGhost = false;
							cheackboxs[rows -1][columns -1].isGhost = true;

							cheackboxs[y][x].color = cheackboxs[y][x].auxPositionColor;
							cheackboxs[rows -1][columns -1].auxPositionColor = cheackboxs[rows -1][columns -1].color;
						}
						else
						{
							cheackboxs[y][x].isGhost = false;
							cheackboxs[y - 1][columns - 1].isGhost = true;

							cheackboxs[y][x].color = cheackboxs[y][x].auxPositionColor;
							cheackboxs[y - 1][columns - 1].auxPositionColor = cheackboxs[y -1][columns - 1].color;
						}
					}
					else
					{
						cheackboxs[y][x].isGhost = false;
						cheackboxs[y][x - 1].isGhost = true;

						cheackboxs[y][x].color = cheackboxs[y][x].auxPositionColor;
						cheackboxs[y][x - 1].auxPositionColor = cheackboxs[y][x - 1].color;
					}
				}

				break;
			}
		}
	}

}
