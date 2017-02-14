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
	void render(int x, int y, loadSDL::Texture renderTexture,SDL_Rect* clip = NULL);

	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The window renderer
	SDL_Renderer* gRenderer = NULL;

	//Texture to load puyo
	Texture gTexturePuyo;

	//Texture to load background
	Texture gTextureBackground;

	//Texture
	Texture gSpriteSheetTexture;
	
	//screen size
	const static int SCREEN_WIDTH = 450;
	const static int SCREEN_HEIGHT = 600;

	SDL_Rect gSpriteClips[4];

};

