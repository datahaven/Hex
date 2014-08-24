// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame
#pragma once

#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_mixer.h"
#include "Board.hpp"
#include "TextResources.hpp"
#include <memory>
#include <utility>
using Coord = std::pair<int, int>;

enum class GameScene {scnIntro, scnMainMenu, scnPlaying, scnThinking};

class HexGame
{
public:
	HexGame();
	~HexGame();
	void GameLoop();
	void Update();
	void RenderGameScene();
	void RenderMenuScene();
	void RenderIntroScene();

	const int WINDOW_X_SIZE = 1024;
	const int WINDOW_Y_SIZE = 768;
	const int LOGICAL_X_SIZE = 1024;
	const int LOGICAL_Y_SIZE = 768;

private:
	SDL_Window *mWin;
	SDL_Renderer *mRen;

	SDL_Texture *mHexTiles;
	SDL_Texture *mHexBoard;
	SDL_Texture *mBackground;

	Mix_Music *mMusic;

	Mix_Chunk *mSndValid;
	Mix_Chunk *mSndInvalid;

	Board mBoard;

	GameScene mCurrentScene;

	std::unique_ptr<TextResources> mpTextResources;

	void RenderBoard(Board &b);
	void RenderCursor();

	void HandleMouseClick(int x, int y);
	void HandleMenuClick(int x, int y);

	// Not really much to do with game - ought to be removed
	// and put in a namespace.
	// Conversion fns need to know where the board is
	static const int mBoardTopLeftX = 75;
	static const int mBoardTopLeftY = 175;
	static Coord MouseToHex(int x, int y);
	static Coord HexToMouse(Coord &hex);

	Uint32 mIntroStartTime;

	const int mMenuX = 600;
	const int mMenuY = 260;
	const int mMenuYSpacing = 60;
	const int mInGameMenuX = 880;
	const int mInGameMenuY = 680;

	bool mQuit = false;
	bool mMusicOn = true;
	bool mSoundOn = true;
	int mWinner = 0;
	bool mIsHuman[3];

	int mCursorHexX = -1;
	int mCursorHexY = -1;

	Uint32 mThinkStartTime;
};