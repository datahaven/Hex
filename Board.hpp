// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame
#pragma once
#include <random>
#include <utility>
#include <vector>

using BoardCell = int;

// Board stores a representation of the game board suitable for 
// determining whose move it is and which cells are occupied by whom.
// Cells are ints 0==empty, 1==plyr1, 2==plyr2
// PlayerToMove() is always 1 or 2
class Board
{
public:
	Board();
	~Board();
	void ResetBoard();
	int GetCell(int x, int y) const;
	void SetCell(int x, int y, const BoardCell &cell);
	int GetPlayerToMove() const;

	// Probably don't need this public - maybe if I implement
	// "Swap Rule", which lets the second player swap colours if he
	// thinks the starting player's move is too strong
	void SetPlayerToMove(int playerNum);
	
	// Choses a move for the current player to move
	void DoComputerMove();

	// Might one day want to do a version with different
	// board sizes?
	static const int BOARD_ROWS = 11;
	static const int BOARD_COLS = 11;
	static const int EDGE_TOP = BOARD_ROWS * BOARD_COLS;
	static const int EDGE_BOTTOM = BOARD_ROWS * BOARD_COLS + 1;
	static const int EDGE_LEFT = BOARD_ROWS * BOARD_COLS + 2;
	static const int EDGE_RIGHT = BOARD_ROWS * BOARD_COLS + 3;

	// Return all possible moves
	std::vector<std::pair<int, int>> GetAllMoves();
	// Has game finished?
	// If yes, will return winning player number.
	// Otherwise returns 0
	int isGameOver();

private:
	int mPlayerToMove;
	std::vector<BoardCell> mCells;

	void MakeConnectionGraph();
	int FloodFill(int cellX, int cellY, int playerNo);
	std::vector<int> mVisited;

	std::mt19937 mRandMT;
};

// Inline member fn defs need to be in a header file
inline int Board::GetCell(int x, int y) const
{
	return mCells[y * BOARD_COLS + x];
}

inline void Board::SetCell(int x, int y, const BoardCell &cell)
{
	mCells[y * BOARD_COLS + x] = cell;
}

inline int Board::GetPlayerToMove() const
{
	return mPlayerToMove;
}

inline void Board::SetPlayerToMove(int playerNum)
{
	mPlayerToMove = playerNum;
}