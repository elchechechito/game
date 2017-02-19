#include <SDL.h>
#include "cheackbox.h"
#include "loadSDL.h"
#pragma once
class board
{
	public:

		struct StructDeletesPuyos {
			int x;
			int y;
			bool empty;
		};

		StructDeletesPuyos deletesPuyos[10];

		int timeDelay;
		int nivel;

		board();
		~board();

		cheackbox cheackboxs[16][11];
		loadSDL sdl;

		void startGameBoard();
		void printBackground(int randomBackground);
		
		void printBoard();

		void gamehandleEvent(SDL_Event& e);

		bool cheackboxIsSelected();

		void moveCheackbox(SDL_Event &e);

		void turnPuyo();
		void moveRight();
		void moveLeft();

		bool moveDown();

		void newPuyos();

		bool endGame();

		bool updateSprite();

		bool checkDeletePuyo();

		void deletePuyos(int x, int y, int originX, int OriginY);

		bool compareBoxes(cheackbox box1, cheackbox box2);


};

