//Project 5, arrays.c Lab Section 1103 by Curtis Chung
#include <stdio.h>
#include <stdbool.h>

int main (void) {
	int row, col, r, c;
	int input;
	int digit = 0;

	printf("This program counts occurences of digits 0 through 9 in an NxM array.\n");
	printf("Enter the size of the array (Row Column): ");
	scanf("%d %d", &row, &col);

	int array[row][col];
	int digitcounter [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	
	for(r = 0; r < row; r++)
	{
		printf("Enter row %d: ", r);
		for(c = 0; c < col; c++)
		{
			scanf("%d", &input);
			array[r][c] = input;
		}
	}


	for(r = 0; r < row; r++)
	{
		for(c = 0; c < col; c++)
		{
			digit = array[r][c];
			switch (digit)
			{
				case 0:
					digitcounter[0] += 1;
					break;
				case 1:
					digitcounter[1] += 1;
					break;
				case 2:
					digitcounter[2] += 1;
					break;
				case 3:
					digitcounter[3] += 1;
					break;
				case 4:
					digitcounter[4] += 1;
					break;
				case 5:
					digitcounter[5] += 1;
					break;
				case 6:
					digitcounter[6] += 1;
					break;
				case 7:
					digitcounter[7] += 1;
					break;
				case 8:
					digitcounter[8] += 1;
					break;
				case 9:
					digitcounter[9] += 1;
					break;
			}
		}
	}
	for (r = 0;r < 10; r++)
	{
		printf("Digit %d occurs %d time\n", r, digitcounter[r]);
	}
	printf("The digit counts directly from the 1D array: \n");
	for (r = 0; r < 10; r++)
	{
		printf("%d ", digitcounter[r]);		
	}
	printf("\n");

	printf("The original 2D array entered by the user:\n");
	for (r = 0; r < row; r++)
	{
		for (c = 0; c < col; c++)
		{
			printf("%d ", array[r][c]);

		}
		printf("\n");
	}

	return 0;
}