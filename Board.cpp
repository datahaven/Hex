// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame
#include "Board.hpp"
#include <ctime>
using namespace std;

// The coordinate offset for the six adjacent hexes
const pair<int, int> AdjCoords[] = {
	{ 0, -1 }, { 1, -1 }, { 1, 0 }, { 0, 1 }, { -1, 1 }, { -1, 0 }
};

const int AdjCells[] = { -3, -2, 1, 3, 2, -1 };

Board::Board()
{
	ResetBoard();
	// Pretty horrible way to pick a random seed!
	mRandMT.seed(static_cast<unsigned long>(time(nullptr)));
}

Board::~Board()
{
}

void Board::ResetBoard()
{
	mCells.clear();
	mCells.reserve(BOARD_ROWS * BOARD_COLS);
	for (int i = 0; i < BOARD_ROWS * BOARD_COLS; ++i)
		mCells.push_back(0);

	mPlayerToMove = 1;
}

vector<pair<int, int>> Board::GetAllMoves()
{
	vector<pair<int, int>> results;
	for (int row = 0; row < Board::BOARD_ROWS; ++row)
	{
		for (int col = 0; col < Board::BOARD_COLS; ++col)
		{
			int piece = GetCell(col, row);
			if (piece == 0)
				results.push_back(make_pair(col, row));
		}
	}
	return results;
}

// Has game finished?
// If yes, will return winning player number.
// Otherwise returns 0
int Board::isGameOver()
{
	MakeConnectionGraph();

	// NB: Draw isn't possible. Can't both be
	// connected at the same time
	if (mVisited[EDGE_TOP] && mVisited[EDGE_BOTTOM]) return 1;
	if (mVisited[EDGE_LEFT] && mVisited[EDGE_RIGHT]) return 2;
	
	return 0;
}


// Walks every populated cell so that we can mark when an edge
// is visited by a player of the matching colour.
// Not the fastest code ever, but ought to do the job for now.
void Board::MakeConnectionGraph()
{
	mVisited.clear();
	// Mark all non-empty cells (including edges) as unvisited.
	// unvisited = 0, visited = 1
	for (int i = 0; i < BOARD_ROWS * BOARD_COLS; ++i)
	{
		mVisited.push_back( mCells[i] == 0 ? 1 : 0);
	}
	// Mark edges as unvisited
	for (int i = 0; i < 4; ++i)
		mVisited.push_back(0);
	
	// Now try flood filling out from every populated cell
	// To improve this it really should just test out from edges.
	for (int row = 0; row < Board::BOARD_ROWS; ++row)
	{
		for (int col = 0; col < Board::BOARD_COLS; ++col)
		{
			int piece = GetCell(col, row);
			if (piece != 0)
				FloodFill(col, row, piece);

			// This little block is a very hacky fix to the problem where
			// having a piece on each edge caused it to register a win, even
			// if the pieces weren't connected.
			// This hack causes us to do a lot of repeated flooding.
			if (mVisited[EDGE_TOP] && mVisited[EDGE_BOTTOM]) return;
			if (mVisited[EDGE_LEFT] && mVisited[EDGE_RIGHT]) return;
			mVisited[EDGE_TOP] = 0;
			mVisited[EDGE_BOTTOM] = 0;
			mVisited[EDGE_LEFT] = 0;
			mVisited[EDGE_RIGHT] = 0;
			for (int &v : mVisited)
				v = 0;
		}
	}
}

int Board::FloodFill(int cellX, int cellY, int playerNo)
{
	int cellNo = cellY * BOARD_COLS + cellX;

	if (mVisited[cellNo] == 1 ) return 0;

	mVisited[cellNo] = 1;

	if (mCells[cellNo] == playerNo)
	{
		for (int i = 0; i < 6; ++i)
		{
			int nextX = cellX + AdjCoords[i].first;
			int nextY = cellY + AdjCoords[i].second;
			// Weird logic because you can only visit an edge of your
			// own colour.
			// However corner cells allow you to visit both edges
			// Does this matter?
			bool edgeVisited = (nextX < 0 || nextX >= Board::BOARD_COLS ||
				nextY < 0 || nextY >= Board::BOARD_ROWS);

			if (nextX < 0)
			{
				if (playerNo == 2)
					mVisited[EDGE_LEFT] = 1;
			}
			else if (nextX >= Board::BOARD_COLS)
			{
				if (playerNo == 2)
					mVisited[EDGE_RIGHT] = 1;
			}
			if (nextY < 0)
			{
				if (playerNo == 1)
					mVisited[EDGE_TOP] = 1;
			}
			else if (nextY >= Board::BOARD_ROWS)
			{
				if (playerNo == 1)
					mVisited[EDGE_BOTTOM] = 1;
			}

			if (edgeVisited == false && GetCell(nextX, nextY) == playerNo)
				FloodFill(nextX, nextY, playerNo);
		}
	}
	return 0;
}

// Choses a move for the current player to move
// Since this has to work pretty much instantly,
// it just chooses a move randomly.
void Board::DoComputerMove()
{
	vector<pair<int, int>> moves = GetAllMoves();
	// check if game is already won, in which case do nothing
	if (moves.size() == 0)
		return;

	// Pick an available move at random.
	// Sorry, _this_ is the AI in its entirety!
	uniform_int_distribution<unsigned int> dist(0, moves.size() - 1);
	auto move = moves[dist(mRandMT)];
	// Make the move
	mCells[move.first + move.second * BOARD_COLS] = mPlayerToMove;

	mPlayerToMove = 3 - mPlayerToMove;
}