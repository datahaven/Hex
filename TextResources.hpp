// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame
#pragma once
#include <stdexcept>
#include <string>
#include <utility>

#include "SDL.h"
#include "SDL_Image.h"

// I made all of the resources in a single bitmap
// I've not got any tools handy for making sprite atlases,
// and even if I did, I've not got code prepared to read them.
// Hence this class which will hard code a lot of the info needed

enum TextResource {tURL=0, tCreator, tOnePlayer, tTwoPlayer, tStart, tOptions, tQuit,
				tVsComputer, tVsHuman, tToPlay, tThinking, tMainLogo, tSubTitle,
				tSound, tOn, tOff, tMusic, tResume, tWins, tMenu, tEND};

const SDL_Rect Atlas[] = {
	// x, y, w, h
	{ 0, 0, 950, 100 }, // tURL
	{ 0, 100, 560, 100 }, // tCreator
	{ 0, 200, 360, 100 }, // tOnePlayer
	{ 0, 300, 364, 100 }, // tTwoPlayer
	{ 0, 400, 170, 100 }, // tStart
	{ 0, 500, 260, 100 }, //tOptions
	{ 0, 600, 140, 100 }, //tQuit
	{ 0, 700, 425, 100 }, //tVsComputer
	{ 0, 800, 330, 100 }, //tVsHuman
	{ 0, 900, 234, 100 }, //, tToPlay, 
	{ 0, 1000, 346, 92 }, //tThinking, 
	{ 1000, 0, 850, 480 }, //tMainLogo
	{ 1000, 500, 880, 100 }, //, tSubTitle
	{ 1000, 600, 235, 100 }, //tSound,
	{ 1000, 700, 105, 100 }, //tOn,
	{ 1000, 800, 105, 100 }, //tOff,
	{ 1000, 900, 230, 100 }, //tMusic
	{ 0, 1100, 230, 100 }, //tResume
	{ 1000, 1000, 230, 92 }, //tWins - not sure why height is wrong. May have messed up bitmap?
	{ 1000, 1100, 230, 100 } //tMenu
};


class TextResources
{
public:
	TextResources(SDL_Renderer *ren) : mRen(ren)
	{
		mTexture = IMG_LoadTexture(mRen, "Resources/TextResources.png");
		if (mTexture == nullptr)
			throw std::runtime_error("IMG_LoadTexture Error: " + std::string{ SDL_GetError() });
	}

	~TextResources()
	{
		if (mTexture != nullptr)
			SDL_DestroyTexture(mTexture);
	}

	void RenderText(TextResource id, int x, int y, double scale = 1.0)
	{
		const SDL_Rect *pSrc = &Atlas[id];
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.w = static_cast<int>(scale * pSrc->w / 2); // because I've made all my resources
		dest.h = static_cast<int>(scale * pSrc->h / 2); // double resolution
		if (SDL_RenderCopy(mRen, mTexture, pSrc, &dest) != 0)
			throw std::runtime_error("SDL_RenderCopy Error: " + std::string{ SDL_GetError() });
	}

private:
	 
	SDL_Texture *mTexture;
	SDL_Renderer *mRen;
};