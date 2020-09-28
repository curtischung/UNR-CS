#include <stdio.h>
#include <tictac_support.h>

int main( int argc, char* argv[] )
{

	int board[3][3], move[3][3];

	// load in current board state
	load_board( argv[1], board );

	// make move
	make_move( board );
	
	// save board state
	save_board( argv[2], board );

	return 0;
}
