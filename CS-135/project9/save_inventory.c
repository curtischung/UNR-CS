//Project 8, Problem 1, save_inventory.c Lab Section 1103

#include <stdio.h>

int main (void) {

	int part_number, quantity;
	float price;
	FILE *fp;
	fp = fopen("inventory.txt", "wb+");

	if (fp == NULL) {
		return 0;
	}

	printf("This program stores a business inventory.\n");
	
	while(1) {
		printf("Please enter item data (part number, quantity, price): ");
			scanf("%d, %d, %f", &part_number ,&quantity, &price);
		if (part_number == 0) {
			break;
		}
//		fprintf(fp, "%d, %d %.2f\n", part_number, quantity, price);
		fwrite(&part_number, sizeof(part_number), 1, fp);
		fwrite(&quantity, sizeof(quantity), 1, fp);
		fwrite(&price, sizeof(price), 1, fp);
		fwrite("\n", sizeof(char), 1, fp);


	}
	fclose(fp);
	printf("Thank you. Inventory stored in file inventory.txt.\n");
return 0;
}
