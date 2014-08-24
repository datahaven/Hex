// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame
#include <memory>
#include <stdexcept>
#include <sstream>
#include "HexGame.hpp"

using namespace std;

HexGame::HexGame()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		throw runtime_error( "SDL_Init Error: " + string{ SDL_GetError() } );

	mWin = SDL_CreateWindow("Hex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_X_SIZE, WINDOW_Y_SIZE, SDL_WINDOW_SHOWN);
	if (mWin == nullptr)
		throw runtime_error("SDL_CreateWindow Error: " + string{ SDL_GetError() } );


	mRen = SDL_CreateRenderer(mWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mRen == nullptr)
		throw runtime_error("SDL_CreateRenderer Error: " + string{ SDL_GetError() } );
	
	// This changes the window size?????
	//if (SDL_RenderSetLogicalSize(mRen, LOGICAL_X_SIZE, LOGICAL_X_SIZE)!=0)
	//	throw runtime_error("SDL_RenderSetLogicalSize Error: " + string{ SDL_GetError() });

	mHexTiles = IMG_LoadTexture(mRen, "Resources/tiles.png");
	if (mHexTiles == nullptr)
		throw runtime_error("IMG_LoadTexture Error: " + string{ SDL_GetError() });

	mBackground = IMG_LoadTexture(mRen, "Resources/Spacescape01.png");
	if (mBackground == nullptr)
		throw runtime_error("IMG_LoadTexture Error: " + string{ SDL_GetError() });

	mHexBoard = IMG_LoadTexture(mRen, "Resources/board.png");
	if (mHexBoard == nullptr)
		throw runtime_error("IMG_LoadTexture Error: " + string{ SDL_GetError() });

	// unique_ptr - no need to worry about deleting this later!
	// Should do this with the SDL types, too
	mpTextResources = make_unique<TextResources>( mRen );

	int audio_rate = 22050;
	int audio_format = AUDIO_S16;
	int audio_channels = 2;
	int audio_buffers = 4096;
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0)
		throw runtime_error("Mix_OpenAudio Error" + string{ SDL_GetError() });

	int audio_volume = MIX_MAX_VOLUME;

	/* Set the music volume */
	Mix_VolumeMusic(audio_volume);

	/* Set the external music player, if any */
	Mix_SetMusicCMD(SDL_getenv("MUSIC_CMD"));

	mMusic = Mix_LoadMUS("Resources/Hex01.wav");
	if (mMusic == nullptr)
		throw runtime_error("Mix_LoadMUS Error" + string{ Mix_GetError() });

	mSndValid = Mix_LoadWAV("Resources/HexSounds_Valid.wav");
	if (mSndValid == nullptr)
		throw runtime_error("Mix_LoadWAV Error" + string{ Mix_GetError() });

	mSndInvalid = Mix_LoadWAV("Resources/HexSounds_Invalid.wav");
	if (mSndInvalid == nullptr)
		throw runtime_error("Mix_LoadWAV Error" + string{ Mix_GetError() });

	Mix_FadeInMusic(mMusic, -1, 2000);
	//Mix_PauseMusic(); // TODO - Remove this!

	mCurrentScene = GameScene::scnIntro;

	mIntroStartTime = SDL_GetTicks();

	mQuit = false;
}

HexGame::~HexGame()
{
	if (mHexTiles != nullptr) 
		SDL_DestroyTexture(mHexTiles);
	if (mBackground != nullptr)
		SDL_DestroyTexture(mBackground);
	if (mHexBoard != nullptr)
		SDL_DestroyTexture(mHexBoard);

	if (mMusic != nullptr)
		Mix_FreeMusic(mMusic);

	if (mSndValid != nullptr)
		Mix_FreeChunk(mSndValid);
	if (mSndInvalid != nullptr)
		Mix_FreeChunk(mSndInvalid);

	Mix_CloseAudio();

	if (mRen != nullptr)
		SDL_DestroyRenderer(mRen);
	if (mWin != nullptr)
		SDL_DestroyWindow(mWin);
}

void HexGame::GameLoop()
{
	while (!mQuit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				mQuit = true;
			}
			if (e.type == SDL_KEYDOWN)
			{
				// Handy hotkey for testing - remove for release
				// mQuit = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				HandleMouseClick(e.motion.x, e.motion.y);
			}
		}


		if (e.type == SDL_MOUSEMOTION)
		{
			auto mouseX = e.motion.x;
			auto mouseY = e.motion.y;
			auto hexPair = MouseToHex(mouseX, mouseY);
			mCursorHexX = hexPair.first;
			mCursorHexY = hexPair.second;

			// Handy test feature for positioning things - remove for release
			// std::stringstream ss;
			// ss << "X: " << mouseX << " Y: " << mouseY << " " << hexPair.first << ", " << hexPair.second;
			// SDL_SetWindowTitle(mWin, ss.str().c_str());
		}

		Update();

		switch (mCurrentScene)
		{
		case GameScene::scnIntro:
			RenderIntroScene();
			break;
		case GameScene::scnMainMenu:
			RenderMenuScene();
			break;
		case GameScene::scnPlaying:
		case GameScene::scnThinking: // Same for both
			RenderGameScene();
			break;
		default:
			throw runtime_error("Current scene is unhandled type");
		};
		

		SDL_Delay(10); // TODO - don't need? RenderPresent should vsync us
	}

}

void HexGame::Update()
{
	// Yes, I know these switch/cases are a sure sign I need
	// separate classes to implement the scenes!
	switch (mCurrentScene)
	{
	case GameScene::scnIntro:
	{
		Uint32 currentTime = SDL_GetTicks();
		if (currentTime - mIntroStartTime > 5000)
			mCurrentScene = GameScene::scnMainMenu;
		break;
	}
	case GameScene::scnThinking:
	{						   
		// A bit of a fudge to make the computer look like it is thinking,
		// when really it'll just quickly pick a move once a delay is up.
		// Otherwise I need an AI algorithm that would work in sync with
		// a game loop
		if (SDL_GetTicks() - mThinkStartTime > 2000)
		{
			mCurrentScene = GameScene::scnPlaying;
			mBoard.DoComputerMove();
			if (mSoundOn)
				Mix_PlayChannel(-1, mSndValid, 0);

			mWinner = mBoard.isGameOver();
		}
		break;
	}
	default:
		break;
	}
	
}

void HexGame::RenderIntroScene()
{
	SDL_SetRenderDrawColor(mRen, 0, 0, 0, 255);
	SDL_RenderClear(mRen);

	SDL_RenderCopy(mRen, mBackground, NULL, NULL);

	mpTextResources->RenderText(tMainLogo, (1024-850)/2, 100, 2);
	mpTextResources->RenderText(tSubTitle, (1024 - 850) / 2, 580, 2);

	mpTextResources->RenderText(tCreator, 875, 730, 0.5);

	SDL_RenderPresent(mRen);
}

void HexGame::RenderMenuScene()
{
	SDL_SetRenderDrawColor(mRen, 0, 0, 0, 255);
	SDL_RenderClear(mRen);

	SDL_RenderCopy(mRen, mBackground, NULL, NULL);

	const int LogoY = 240;
	mpTextResources->RenderText(tMainLogo, (1024 - 850) / 2, LogoY);
	mpTextResources->RenderText(tSubTitle, (1024 - 860) / 2, LogoY+240);

	mpTextResources->RenderText(tCreator, 875, 730, 0.5);

//	tURL = 0, tCreator, tOnePlayer, tTwoPlayer, tStart, tOptions, tQuit,
//		tVsComputer, tVsHuman, tToPlay, tThinking, tMainLogo, tSubTitle,
//		tSound, tOn, tOff, tMusic, tEND

	const int MenuX = mMenuX;
	int MenuY = mMenuY;
	const int MenuYSpacing = mMenuYSpacing;
	mpTextResources->RenderText(tOnePlayer, MenuX, MenuY);
	MenuY += MenuYSpacing;
	mpTextResources->RenderText(tTwoPlayer, MenuX, MenuY);
	MenuY += MenuYSpacing;
	//mpTextResources->RenderText(tStart, MenuX, MenuY);
	//MenuY += MenuYSpacing;
	mpTextResources->RenderText(tSound, MenuX, MenuY);
	mpTextResources->RenderText(mSoundOn ? tOn : tOff, MenuX + 120, MenuY);
	MenuY += MenuYSpacing;

	mpTextResources->RenderText(tMusic, MenuX, MenuY);
	mpTextResources->RenderText(mMusicOn ? tOn : tOff, MenuX + 115, MenuY);
	MenuY += MenuYSpacing;
	
	mpTextResources->RenderText(tQuit, MenuX, MenuY);
	MenuY += MenuYSpacing;
	
	mpTextResources->RenderText(tURL, 10, 730, 0.5);

	SDL_RenderPresent(mRen);
}

void HexGame::RenderGameScene()
{
	SDL_SetRenderDrawColor(mRen, 0, 0, 0, 255);
	SDL_RenderClear(mRen);

	SDL_RenderCopy(mRen, mBackground, NULL, NULL);

	RenderBoard(mBoard);

	mpTextResources->RenderText(tMainLogo, 750, 35, 0.6);
	mpTextResources->RenderText(tSubTitle, 745, 180, 0.6);

	mpTextResources->RenderText(tCreator, 875, 730, 0.5);

	SDL_RenderPresent(mRen);
}

void HexGame::RenderBoard(Board &b)
{
	// Using QueryTexture to get size of the board - could just hardcode it,
	// but that's likely to cause problems
	SDL_Rect bdest;
	if (SDL_QueryTexture(mHexBoard, nullptr, nullptr, &bdest.w, &bdest.h) != 0)
		throw runtime_error("SDL_QueryTexture Error: " + string{ SDL_GetError() });
	bdest.x = mBoardTopLeftX - 54;
	bdest.y = mBoardTopLeftY - 27;
	bdest.w /= 2;
	bdest.h /= 2;
	if (SDL_RenderCopy(mRen, mHexBoard, nullptr, &bdest) != 0)
		throw runtime_error("SDL_RenderCopy Error: " + string{ SDL_GetError() });

	SDL_Rect src;
	src.h = 108;
	src.w = 108;
	src.x = 0;
	src.y = 0;
	SDL_Rect dest;
	dest.h = 54; dest.w = 54;
	dest.x = mBoardTopLeftX;
	dest.y = mBoardTopLeftY;

	int xoffset = mBoardTopLeftX;
	int yoffset = mBoardTopLeftY;
	for (int row = 0; row < Board::BOARD_ROWS; ++row)
	{
		for (int col = 0; col < Board::BOARD_COLS; ++col)
		{
			int piece = b.GetCell(col, row);
			if (piece == 0)
				continue;

			src.x = piece * 108;

			dest.x = xoffset + col * 54;
			dest.y = yoffset;
			if (SDL_RenderCopy(mRen, mHexTiles, &src, &dest) != 0)
				throw runtime_error("SDL_RenderCopy Error: " + string{ SDL_GetError() });
		}
		xoffset += 27;
		yoffset += 40;
	}

	// Render the cursor on top of the board
	RenderCursor();
	
	// Render the marker to indicate whose turn it is
	const int MarkerX = 50;
	const int MarkerY = 700;
	int nextPieceToMove = mBoard.GetPlayerToMove();
	// Need this since by the time someone has won, move indicator has
	// already been swapped to next player, who is, of course, the loser,
	// not the winner
	if (mWinner != 0)
		nextPieceToMove = mWinner;
	src.y = 0;
	src.x = nextPieceToMove * 108;
	src.h = 108;
	src.w = 108;
	dest.h = 54;
	dest.w = 54;
	dest.x = MarkerX;
	dest.y = MarkerY;
	if (SDL_RenderCopy(mRen, mHexTiles, &src, &dest) != 0)
		throw runtime_error("SDL_RenderCopy Error: " + string{ SDL_GetError() });

	if (mWinner != 0)
	{
		mpTextResources->RenderText(tWins, MarkerX + 60, MarkerY + 8);
	}
	else
	{
		mpTextResources->RenderText(tToPlay, MarkerX + 60, MarkerY + 8);
	}

	mpTextResources->RenderText(tMenu, mInGameMenuX, mInGameMenuY, 0.5);

	// Also make sure game isn't already over - a bug I fixed.
	if (mIsHuman[mBoard.GetPlayerToMove()] == false && mWinner == 0)
	{
		mpTextResources->RenderText(tThinking, MarkerX, MarkerY - 100, 1);
	}
}

void HexGame::RenderCursor()
{
	// Bug fix - remove the cursor if it's not a human's turn to play
	if (mCursorHexX == -1 || mCursorHexY == -1 ||
		mIsHuman[mBoard.GetPlayerToMove()] == false)
		return;

	SDL_Rect src;
	src.h = 108;
	src.w = 108;
	src.x = (2 + mBoard.GetPlayerToMove())*108; // Cursors are 4th/5th cell tile
	src.y = 0;                                  // one for each player
	SDL_Rect dest;
	dest.h = 54; dest.w = 54;
	dest.x = mBoardTopLeftX;
	dest.y = mBoardTopLeftY;

	int xoffset = mBoardTopLeftX;
	int yoffset = mBoardTopLeftY;
	for (int row = 0; row < Board::BOARD_ROWS; ++row)
	{
		for (int col = 0; col < Board::BOARD_COLS; ++col)
		{
			if (row == mCursorHexY && col == mCursorHexX && mBoard.GetCell(mCursorHexX, mCursorHexY)==0)
			{
				// WTF is this loop! Lazy! Can calculate the position directly!
				// Not enough time ... priorities ... blah ...
				dest.x = xoffset + col * 54;
				dest.y = yoffset;
				if (SDL_RenderCopy(mRen, mHexTiles, &src, &dest) != 0)
					throw runtime_error("SDL_RenderCopy Error: " + string{ SDL_GetError() });
			}
		}
		xoffset += 27;
		yoffset += 40;
	}

}
Coord HexGame::MouseToHex(int mx, int my)
{
	Coord c = make_pair(-1, -1);
	/*
	if (my < mBoardTopLeftY ||
		my > mBoardTopLeftY + 40 + Board::BOARD_ROWS * 40 ||
		mx < mBoardTopLeftX ||
		mx > mBoardTopLeftX + 54 * Board::BOARD_COLS)
		return c;
	*/

	if (mx < mBoardTopLeftX || my < mBoardTopLeftY)
		return c;

	mx -= mBoardTopLeftX;
	my -= mBoardTopLeftY;

	int cy = my / 40;
	int cx = (mx-27*cy) / 54;

	if (cy >= Board::BOARD_ROWS || cx >= Board::BOARD_COLS)
		return c;

	c.first = cx;
	c.second = cy;
	return c;
}

// Don't need this
Coord HexToMouse(Coord &hex)
{
	Coord c;
	return c;
}

void HexGame::HandleMenuClick(int mouseX, int mouseY)
{
	if (mouseX > mMenuX && mouseX < mMenuX + 200)
	{
		if (mouseY > mMenuY && mouseY < mMenuY + mMenuYSpacing)
		{
			// One Player Start
			mIsHuman[1] = true;
			mIsHuman[2] = false;
			mCurrentScene = GameScene::scnPlaying;
			mWinner = 0;
			mBoard.ResetBoard();
		}
		if (mouseY > mMenuY + mMenuYSpacing && mouseY < mMenuY + 2 * mMenuYSpacing)
		{
			// Two Player Start
			mCurrentScene = GameScene::scnPlaying;
			mWinner = 0;
			mIsHuman[1] = true;
			mIsHuman[2] = true;
			mBoard.ResetBoard();
		}
		if (mouseY > mMenuY + 2* mMenuYSpacing && mouseY < mMenuY + 3 * mMenuYSpacing)
		{
			// Sound Toggle
			mSoundOn = !mSoundOn;
		}
		if (mouseY > mMenuY + 3* mMenuYSpacing && mouseY < mMenuY + 4 * mMenuYSpacing)
		{
			// Music Toggle
			if (Mix_PausedMusic())
			{
				Mix_ResumeMusic();
				mMusicOn = true;
			}
			else
			{
				Mix_PauseMusic();
				mMusicOn = false;
			}
		}
		if (mouseY > mMenuY + 4* mMenuYSpacing && mouseY < mMenuY + 5 * mMenuYSpacing)
		{
			// Quit
			mQuit = true;
			return;
		}
	}
}

void HexGame::HandleMouseClick(int mouseX, int mouseY)
{
	// State handling.
	// ie check we're actually ingame.
	// This class will need to be broken out somehow, I think.
	if (mCurrentScene == GameScene::scnMainMenu)
	{
		HandleMenuClick(mouseX, mouseY);
		return;
	}
	else if (mCurrentScene == GameScene::scnIntro)
	{
		mCurrentScene = GameScene::scnMainMenu;
		return;
	}

	// else we're processing a GameScene::scnMainGame click:

	if (mouseX >= mInGameMenuX && mouseX < mInGameMenuX + 115 &&
		mouseY >= mInGameMenuY && mouseY < mInGameMenuY + 50)
	{
		mCurrentScene = GameScene::scnMainMenu;
		return;
	}

	// Thinking is a sub-scene of scnMainGame, really
	if (mCurrentScene == GameScene::scnThinking)
	{
		// Fixes a bug where we could play computer's moves
		// during his turn.
		return;
	}
	

	if (mWinner != 0)
	{
		// No more moves possible if we have a winner
		return;
	}


	auto hexPair = MouseToHex(mouseX, mouseY);
	if (hexPair.first == -1)
	{
		// Clicked outside grid
		if (mSoundOn)
			Mix_PlayChannel(-1, mSndInvalid, 0);
		return;
	}

	int cellAtPos = mBoard.GetCell(hexPair.first, hexPair.second);
	if (cellAtPos != 0)
	{
		// Clicked on a piece that's already there
		if (mSoundOn)
			Mix_PlayChannel(-1, mSndInvalid, 0);
		return;
	}

	// Make our move
	int ptm = mBoard.GetPlayerToMove();
	mBoard.SetCell(hexPair.first, hexPair.second, ptm);
	mBoard.SetPlayerToMove(3 - ptm);
	if (mSoundOn)
		Mix_PlayChannel(-1, mSndValid, 0);

	mWinner = mBoard.isGameOver();
	
	// Get ready for next player
	if (mWinner == 0 && mIsHuman[mBoard.GetPlayerToMove()] == false)
	{
		// If it is computer then start it's thinking
		mThinkStartTime = SDL_GetTicks();
		mCurrentScene = GameScene::scnThinking;
	}

}