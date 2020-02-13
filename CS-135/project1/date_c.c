//Date_c.c Project 1 (Extra Credit) by Curtis Chung, Lab Section 1103
#include <stdio.h>

int main(void)
{
	int day, month, year;
	int day2, month2, year2;
	int day3, month3, year3;
	int day4, month4, year4;
		printf("Enter date 1 (dd/mm/yyyy): \n"); 
			scanf("%d/%d/%d", &day, &month, &year);

		printf("Enter date 2 (dd/mm/yyyy): \n"); 
			scanf("%d/%d/%d", &day2, &month2, &year2);

		printf("Enter date 3 (dd/mm/yyyy): \n"); 
			scanf("%d/%d/%d", &day3, &month3, &year3);

		printf("Enter date 4 (dd/mm/yyyy): \n"); 
			scanf("%d/%d/%d", &day4, &month4, &year4);

		printf("\tYear \tMonth \tDay \n");
			printf("\t%04d \t%02d \t%02d\n", year, month, day);

			printf("\t%04d \t%02d \t%02d\n", year2, month2, day2);

			printf("\t%04d \t%02d \t%02d\n", year3, month3, day3);

			printf("\t%04d \t%02d \t%02d\n", year4, month4, day4);
	return 0;
}