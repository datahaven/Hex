// Adrian Dale
// 23/08/2014
// LD #30 - Connected Worlds
// Hex Boardgame

#include <iostream>
#include "HexGame.hpp"
#include "SDL.h"

using namespace std;

int main(int argc, char ** argv)
{
	cout << "Hex Board Game\n";

	try
	{
		HexGame hg;
		hg.GameLoop();
	}
	catch (exception &e)
	{
		cerr << "EXCEPTION: " << e.what() << endl;
		return -1;
	}

	cout << "Game Exited Successfully!\n";

	return 0;
}