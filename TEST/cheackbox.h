#include <SDL.h>

#pragma once
class cheackbox
{
public:
	cheackbox();
	~cheackbox();

	SDL_Rect box;

	bool isFree;

	int color;
	int sprite;

	bool isSelected;
	bool isBase;
	bool isFalling;

};

