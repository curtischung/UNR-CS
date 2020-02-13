//Project 9, Problem 2, disp_inventory.c Lab Section 1103

#include <stdio.h>

int main (int argc, char *argv[]) {

	int i, space;
	int part_number, quantity;
	float price;
	FILE *fp;
	fp = fopen("inventory.txt", "rb");

	if (fp == NULL) {
		return 0;
	}
	printf("Below are the items in your inventory.\n");
		printf("Part# \tQuantity\tItem Price\n");

	while(1) {
		char c = 'a';
		//int n = fscanf(fp, "%d, %d %f", &part_number, &quantity, &price);
		fread(&part_number, sizeof(part_number), 1, fp);
		fread(&quantity, sizeof(quantity), 1, fp);
		fread(&price, sizeof(price), 1, fp);
		fread(&c, sizeof(char), 1, fp);
			fwrite("\n", sizeof(char), 1, fp);
		if(c == '\n') {
			printf("%5d\t%8d\t$%9.2f\n", part_number, quantity, price);
		} else {
			break;
		}
	}
	fclose(fp);
	return 0;

}