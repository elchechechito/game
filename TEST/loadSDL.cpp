#include "loadSDL.h"

loadSDL::loadSDL()
{
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The window renderer
	SDL_Renderer* gRenderer = NULL;
}

loadSDL::~loadSDL()
{
}

bool loadSDL::init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
	
}

void loadSDL::close()
{
	//Free loaded images
	//gTexturePuyo.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool loadSDL::loadMedia()
{
	//Loading success flag
	bool success = true;

	gTexturePuyo = loadTexture("img/puyoSprite2.png");

	//Load sprite sheet texture
	if (gTexturePuyo.texture == NULL)
	{
		printf("No se cargo la imagen de puyo!\n");
		success = false;
	}
	else
	{
		//cargar puyos rojos
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			redPuyoSprite[i].x = i * 32;
			redPuyoSprite[i].y = 0;
			redPuyoSprite[i].w = 33;
			redPuyoSprite[i].h = 31;
		}

		//cargar puyos verdes
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			greenPuyoSprite[i].x = i * 32;
			greenPuyoSprite[i].y = 40;
			greenPuyoSprite[i].w = 33;
			greenPuyoSprite[i].h = 31;
		}

		//cargar puyos azul
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			bluePuyoSprite[i].x = i * 32;
			bluePuyoSprite[i].y = 79;
			bluePuyoSprite[i].w = 33;
			bluePuyoSprite[i].h = 31;
		}

		//cargar puyos amarillos
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			yellowPuyoSprite[i].x = i * 32;
			yellowPuyoSprite[i].y = 120;
			yellowPuyoSprite[i].w = 33;
			yellowPuyoSprite[i].h = 32;
		}

		//cargar puyos morados
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			purplePuyoSprite[i].x = i * 32;
			purplePuyoSprite[i].y = 160;
			purplePuyoSprite[i].w = 33;
			purplePuyoSprite[i].h = 32;
		}

		//cargar puyos rosados
		for (int i = 0; i < 17; i++)
		{
			//Set sprite clips
			pinkPuyoSprite[i].x = i * 32;
			pinkPuyoSprite[i].y = 200;
			pinkPuyoSprite[i].w = 33;
			pinkPuyoSprite[i].h = 31;
		}

	}

	gTextureBackground = loadTexture("img/background.png");

	//Load sprite sheet texture
	if (gTextureBackground.texture == NULL)
	{
		printf("No se cargo el fondo!\n");
		success = false;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			backgroundSprite[i].x = i * 260;
			backgroundSprite[i].y = 0;
			backgroundSprite[i].w = 255;
			backgroundSprite[i].h = 195;
		}
	}

	return success;
}

loadSDL::Texture loadSDL::loadTexture(std::string path)
{
	Texture loadTexture;

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			loadTexture.textureWidth = loadedSurface->w;
			loadTexture.textureHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	loadTexture.texture = newTexture;

	return loadTexture;
}

void loadSDL::render(int x, int y, loadSDL::Texture renderTexture, SDL_Rect * clip, bool screenSize = false)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, renderTexture.textureWidth, renderTexture.textureHeight };
	
	//Set clip rendering dimensions
	if (clip != NULL && !screenSize)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	else if (screenSize)
	{
		renderQuad.w = SCREEN_WIDTH;
		renderQuad.h = SCREEN_HEIGHT;
	}
	
	//Render to screen
	SDL_RenderCopy(gRenderer, renderTexture.texture, clip, &renderQuad);
}
