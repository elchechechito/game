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

			if (x == 2)
			{
				cheackbox[y][x].isFree = false;
				cheackbox[y][x].color = 5;
				cheackbox[y][x].sprite = 3;
			}
			else
			{
				cheackbox[y][x].isFree = true;
			}
		}
	}
}


board::~board()
{
}
