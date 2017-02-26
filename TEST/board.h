#include <SDL.h>
#include "cheackbox.h"
#include "loadSDL.h"

class board
{
public:

	board();
	~board();

	int m_timestamp;
	int m_timestampPC;

	int rows;
	int columns;

	struct StructDeletesPuyos {
		int x;
		int y;
		bool empty;
		bool visited;
	};

	StructDeletesPuyos deletesPuyos[10];

	cheackbox cheackboxs[16][11];
	cheackbox cheackboxs2[16][11];

	cheackbox cheackboxsAux[16][11];
	loadSDL sdl;

	int timeDelay;
	int timeDelayPC;

	int combos;
	int combosPC;

	bool changeDown;

	void startGameBoard();
	void printBackground(int randomBackground);

	void printBoard();

	void gamehandleEvent(SDL_Event& e);

	bool cheackboxIsSelected();

	void moveCheackbox(SDL_Event &e);

	void turnPuyo();
	void turnPuyoPC();

	void moveRight();
	void moveRightPC();

	void moveLeft();
	void moveLeftPC();

	bool moveDown();

	bool moveDownPC();

	void newPuyos();

	void newPuyosPC();

	bool endGame();

	bool updateSprite();

	bool updateSpritePC();

	bool checkDeletePuyo();

	bool checkDeletePuyoPC();

	void deletePuyos(int x, int y);

	void deletePuyosPC(int x, int y);

	bool isVisited(int x, int y);

	void flipBoard();

	void changePuyo();

};