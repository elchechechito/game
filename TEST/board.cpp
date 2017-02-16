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

			////Random background
			int isFreeRandom = rand() % 100 > 50 ? 1 : 0;

			if (isFreeRandom == 1)
			{
				cheackbox[y][x].isFree = false;
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
