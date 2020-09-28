#include <tictac_support.h>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <algorithm>

using namespace std;

#define ROW 3
#define COL 3

int minimax (int depth, int board[][3], bool isMax, int state);
int checkWin (int board [][3]);
bool checkFull(int board[][3]);
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
		(current implementation returns 1 by default, 0 if no move made)
**/
bool checkFull(int board[][3])
{
	//returns true if board is full, false if it is not
	for(int i = 0; i < ROW; i++)
	{
		for(int j = 0; j < COL; j++)
		{
			if(board[i][j] == 0)
			{
				return false;
			}
		}
	}
	return true;
}

int minimax (int depth, int board[][3], bool isMax, int state)
{


	int result = checkWin(board);
	//check if there is a winner
	//returns 1 if there is, 0 if there is no winner
	if (result != 0)
	{
		return result * state;
	}

	if(checkFull(board))
	{
		return 0;
	}

	//maximizer's turn
	if(isMax)
	{
		int best = INT_MIN;
		for(int i = 0; i < ROW; i++) {
			for(int j = 0; j < COL; j++) {
				if(board[i][j] == 0)
				{
					board[i][j] = state;
					best = std::max(best, minimax(depth + 1, board, !isMax, state));
					board[i][j] = 0;

				}
			}
		}
		return best;
	}
	//minimizers turn
	else
	{
		int best = INT_MAX;
		for(int i = 0; i < ROW; i++) {
			for(int j = 0; j < COL; j++) {
				if(board[i][j] == 0)
				{
					board[i][j] = -state;
					best = std::min(best, minimax(depth + 1, board, !isMax, state));
					board[i][j] = 0;

				}
			}
		}
		return best;
	}
}

int checkWin (int board [][3])
{
	//positive is player1, negative is player2
	
	//check for win in each row
	for(int i = 0; i < ROW; i++) 
	{
		if(board[i][0] == board[i][1] && board[i][1] == board[i][2])
		{
			if(board[i][0] == 1)
			{
				return 10;
			}
			if(board[i][0] == -1)
			{
				return -10;
			}
		}
	}

	//check for win in each column
	for(int i = 0; i < COL; i++) 
	{
		if(board[0][i] == board[1][i] && board[1][i] == board[2][i])
		{
			if(board[0][i] == 1)
			{
				return 10;
			}
			if(board[0][i] == -1)
			{
				return -10;
			}
		}
	}

	//check diagnol wins
	if(board[0][0] == board[1][1] && board[1][1] == board[2][2])
	{
		if(board[0][0] == 1)
		{
			return 10;
		}
		if(board[0][0] == -1)
		{
			return -10;
		}
	}

	if(board[0][2] == board[1][1] && board[2][0] == board[2][2])
	{
		if(board[0][2] == 1)
		{
			return 10;
		}
		if(board[0][2] == -1)
		{
			return -10;
		}
	}
	//if tie
	return 0;
}
int make_move( int board[][3] )
{
	int move = INT_MIN;
	int maxRow = 0;
	int maxCol = 0;
	// figure out what move it is
	int state = 0;
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			state += board[i][j];

	state = -state;
	if( state == 0 )
		state = 1;

	// default behavior: find any unoccupied square and make the move
	for( int i = 0; i < 3; i++ ) {
		for( int j = 0; j < 3; j++ ) {
			// find an empty square
			if( board[i][j] == 0 )
			{
				// that's the move
				board[i][j] = state;
				//call minimax
				int best = minimax(0, board, false, state);
				//undo the move
				board[i][j] = 0;

				//check if the move's "best" value is higher than the current "move" value
				if (best > move)
				{
					//if it is higher, replace move with the new best move values
					move = best;
					maxRow = i;
					maxCol = j;
				}
			}
		}
	}
	printf( "player [%d] made move: [%d,%d]\n", state, maxRow, maxCol );
	board[maxRow][maxCol] = state;
	// no move was made (board was full)
	return 1;
}
