#ifndef _TICTAC_SUPPORT_H_
#define _TICTAC_SUPPORT_H_

#include <string>

/**
	load_board: loads a tic-tac-toe board state from a file

	args:
		std::string filename: file to read the board from
		int [][3] board: 3x3 array of ints representing the board state
			0: empty
			1: x
		 -1: o

	returns (int):
		0: if file loaded OK
		-1: if file doesn't exist

**/

int load_board( std::string filename, int board[][3] );


/**
	save_board: saves a tic-tac-toe board state to a file

	args:
		std::string filename: file to write the board to
		int [][3] board: 3x3 array of ints representing the board state
			0: empty
			1: x
		 -1: o

	returns (int):
		0: if file opened OK
		-1: if file can't be opened

**/

int save_board( std::string filename, int board[][3] );


/**
	make_move: takes a board state and makes a legal 
	(hopefully optimal) move

	args:
		int [][3] board: 3x3 array of ints representing the 
		board state. The values of board are altered based
		on the move
			0: empty
			1: x
		 -1: o
		
	returns (int):
		the number of steps it took to choose the best move
		(returns 1 by default, 0 if no move made)
**/

int make_move( int board[][3] );
#endif
