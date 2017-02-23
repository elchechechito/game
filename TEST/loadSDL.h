#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#pragma once
class loadSDL
{
public:
	
	struct Texture {
		SDL_Texture* texture;
		int textureWidth;
		int textureHeight;
	};
	
	loadSDL();
	~loadSDL();

	//Starts up SDL and creates window
	bool init();

	//Frees media and shuts down SDL
	void close();

	//Loads media
	bool loadMedia();

	//Load Texture
	Texture loadTexture(std::string path);

	//Renders texture at given point
	void render(int x, int y, loadSDL::Texture renderTexture,SDL_Rect* clip, bool screenSize);

	//screen size
	const static int SCREEN_WIDTH = 475;
	const static int SCREEN_HEIGHT = 600;

	//The window we'll be rendering to
	SDL_Window* gWindow;

	//The window renderer
	SDL_Renderer* gRenderer;

	//Texture to load puyo
	Texture gTexturePuyo;
	SDL_Rect redPuyoSprite[17];
	SDL_Rect greenPuyoSprite[17];
	SDL_Rect bluePuyoSprite[17];
	SDL_Rect yellowPuyoSprite[17];
	SDL_Rect purplePuyoSprite[17];
	SDL_Rect pinkPuyoSprite[17];
	SDL_Rect badPuyoSprite;

	//Texture to load background
	Texture gTextureBackground;
	SDL_Rect backgroundSprite[5];
		
};

