//Project 6 Tic Tac Toe by Curtis Chung, Lab Section 1103
#include <stdio.h>
#include <stdbool.h>

#define CLEARINPUT (0)
#define SIZE (3)

void display_table (char board[SIZE][SIZE]);//check
void clear_table (char board[SIZE][SIZE]);//check
void get_player1_move (char board[SIZE][SIZE], int row, int col);//check
void generate_player2_move (char board[SIZE][SIZE], int row, int col);//check
void update_table (char board[SIZE][SIZE], int row, int col, int token);//check
void print_winner (char board[SIZE][SIZE]);// check
int check_table_full (char board[SIZE][SIZE]); //CHECK
int check_legal_option(char board[SIZE][SIZE], int row, int col); //check
int check_three_in_a_row(char board[SIZE][SIZE]); //?
int check_end_of_game (char board[SIZE][SIZE]); //check



int main (void) {
	char board [SIZE][SIZE];
	int row, col;

	clear_table (board);
	display_table(board);

	             
	do {
		get_player1_move(board, row, col);
		generate_player2_move(board, row, col);
	} while (check_end_of_game(board) == false); {
		print_winner(board);
	}


}

void display_table (char board[SIZE][SIZE]) {
	int r, c;
	printf("The current state of the game is:\n");
	for (r = 0; r < SIZE; r++) {
		for (c = 0; c < SIZE; c++) {
			printf("%c ", board[r][c]);
		}
		printf("\n");
	}
}

void clear_table (char board[SIZE][SIZE]) {
	int r, c;
	for (r = 0; r < 3; r++) {
		for (c = 0; c < 3; c++) {
			board[r][c] = '_';
		}
	}
}

int check_table_full (char board[SIZE][SIZE]) {
	int r, c, input = 0;
		for (r = 0; r < SIZE; r++) {
			for (c = 0; c < SIZE; c++) {
				if (board[SIZE][SIZE] == '_') {
					return false;
				} else if (board[r][c] == 'X' || board[r][c] == 'O') {
					input++;
				} 
				if (input == 9) {
					return true;
				}				
			}
		} 
}

void update_table (char board[SIZE][SIZE], int row, int col, int token) {
	if (token == 0) {
		board[row][col] = 'O';
	} else if (token == 1) {
		board[row][col] = 'X';
	}
}

int check_legal_option (char board[SIZE][SIZE], int row, int col) {
		if (board[row][col] == 'X' || board[row][col] == 'O') {
			return false;
		} else {
			return true;
		}
}

void generate_player2_move (char board[SIZE][SIZE], int row, int col) {
	int token = 1;
	if (check_table_full(board) == true) {
		return;
		}
	if (check_three_in_a_row(board) == 1 || check_three_in_a_row(board) == 2) {
		return;
	}
	srand(time(NULL));
	row = rand() % SIZE;
	col = rand() % SIZE;
	while (check_legal_option(board, row, col) == false) {
		row = rand() % SIZE;
		col = rand() % SIZE;
	}

	if (check_legal_option(board, row, col) == true) {
		printf("Player 2 has entered [row, col]: %d, %d\n", ++row, ++col);
		row--;
		col--;
		update_table(board, row, col, token);
		display_table(board);
	}
}

void get_player1_move (char board[SIZE][SIZE], int row, int col) {
	int token = 0;
	printf("Player 1 enter your selection [row, col]: ");
			scanf("%d, %d", &row, &col);
			row--;
			col--;
	while (row >= 3 || col >= 3) {
		printf("Invalid Option: Please choose a legal option [row, col]: ");
			scanf("%d, %d", &row, &col);
			row--;
			col--;
	}
	while (check_legal_option(board, row, col) == false) {
		printf("Invalid Option: Please choose a legal option [row, col]: ");
			scanf("%d, %d", &row, &col);
			row--;
			col--;
	}
	update_table(board, row, col, token);
	display_table(board);
	if (check_three_in_a_row(board) == 1 || check_three_in_a_row(board) == 2) {
		return;
	}
	if (check_table_full(board) == true) {
		return;
	}
}	


int check_three_in_a_row(char board[SIZE][SIZE]) {
	if (board[0][0] == 'O' && board [1][0] == 'O' && board[2][0] == 'O') { //Column 1 Check
		return 1;
	}
	if (board[0][1] == 'O' && board [1][1] == 'O' && board[2][1] == 'O') { //Column 2 Check
		return 1;
	}
	if (board[0][2] == 'O' && board [1][2] == 'O' && board[2][2] == 'O') { // Column 3 Check
		return 1;
	}
	if (board[0][0] == 'X' && board [1][0] == 'X' && board[2][0] == 'X') {
		return 2;
	}
	if (board[0][1] == 'X' && board [1][1] == 'X' && board[2][1] == 'X') {
		return 2;
	}
	if (board[0][2] == 'X' && board [1][2] == 'X' && board[2][2] == 'X') {
		return 2;
	}
	if (board[0][0] == 'O' && board [0][1] == 'O' && board[0][2] == 'O') { //Row 1 Check
		return 1;
	}
	if (board[1][0] == 'O' && board [1][1] == 'O' && board[1][2] == 'O') { //Row 2 Check
		return 1;
	}
	if (board[2][0] == 'O' && board [2][1] == 'O' && board[2][2] == 'O') { //Row 3 Check
		return 1;
	}
	if (board[0][0] == 'X' && board [0][1] == 'X' && board[0][2] == 'X') {
		return 2;
	}
	if (board[1][0] == 'X' && board [1][1] == 'X' && board[1][2] == 'X') {
		return 2;
	}
	if (board[2][0] == 'X' && board [2][1] == 'X' && board[2][2] == 'X') {
		return 2;
	}
	if (board[0][0] == 'O' && board[1][1] == 'O' && board[2][2] == 'O') { //Diagnol Checks
			return 1;
	}
	if (board[0][2] == 'O' && board[1][1] == 'O' && board[2][0] == 'O') {
		return 1;
	}
	if (board[0][0] == 'X' && board[1][1] == 'X' && board[2][2] == 'X') {
		return 2;
	}
	if (board[0][2] == 'X' && board[1][1] == 'X' && board[2][0] == 'X') {
		return 2;
	} else {
		return 0;
	}
}

int check_end_of_game (char board[SIZE][SIZE]) {
	if (check_three_in_a_row(board) == 1 || check_three_in_a_row(board) == 2) {
		return true;
	} else if (check_table_full(board) == 1) {
		return true;
	} else {
		return false;
	}
}

void print_winner (char board[SIZE][SIZE]) {
	if (check_three_in_a_row(board) == 1) {
		printf("Congratulations, Player 1 wins!\n");
	}
	if (check_three_in_a_row(board) == 2) {
		printf("Congratulations, Player 2 wins!\n");
	}	
	if (check_table_full(board) == true && check_three_in_a_row(board) == 0) {
		printf("Game over, no player wins!\n");
	}
}