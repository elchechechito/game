#include <SDL.h>
#include "cheackbox.h"
#include "loadSDL.h"
#pragma once
class board
{
	public:
		board();
		~board();

		cheackbox cheackbox[16][11];
		loadSDL sdl;

		void startGameBoard();
		void printBackground(int randomBackground);
		
		void printBoard();

		void gamehandleEvent(SDL_Event& e);

		bool cheackboxIsSelected();

		void moveCheackbox(SDL_Event &e);

		void turnPuyo();

};

