//Collections.c Project 2, By Curtis Chung, Lab Section 1103
#include <stdio.h>
#include <stdbool.h>

int main (void)
{
	int collection1, collection2, collection3, quantity1, quantity2, quantity3, budget;
	int collectiontotalprice, collection1price, collection2price, collection3price;

	printf("Enter volumes and price per volume for collection 1: ");
	scanf("%d %d", &collection1, &quantity1);

	printf("Enter volumes and price per volume for collection 2: ");
	scanf("%d %d", &collection2, &quantity2);

	printf("Enter volumes and price per volume for collection 3: ");
	scanf("%d %d", &collection3, &quantity3);

	printf("Enter Bob's budget: ");
	scanf ("%d", &budget);

	collection1price = collection1 * quantity1;
	collection2price = collection2 * quantity2;
	collection3price = collection3 * quantity3;

	collectiontotalprice = (collection1 * quantity1) + (collection2 * quantity2) + (collection3 * quantity3);


		if( budget > 0 ){ /*Checking Budget is greater than 0*/
			printf("Bob has some money to buy collections.\n");
		} else {
			printf("Bob does not have money to buy anything.\n");
		}

		if( budget >= collectiontotalprice ){ /*Checking if budget is able to afford all of the collections.*/
			printf("Bob has enough money to buy all three collections.\n");
		} else {
			printf("Bob does not have enough money to buy all three collections.\n");
		}

		if((budget <= collection1price && budget <= collection2price) || (budget <= collection3price && budget <=collection1price) || (budget <= collection2price && budget <= collection3price)){ /*Checking if 2 or more collection prices are at least less than or equal to bobs bidget*/
			printf("At least two collections are more expensive than Bob's budget.\n");
		} else {
			printf("At least two collections are cheaper than or equal to Bob's budget.\n");
		}

		if(collection1price == collection2price || collection2price == collection3price || collection3price == collection1price){
			printf("At least two collections cost the same amount of money.\n");
		} else {
			printf("No two collections have the same price.\n");
		}
		if((budget < collection1price && budget < collection2price) || (budget < collection2price && budget < collection3price) || (budget < collection3price && budget < collection1price)){
			printf("Only one collection is cheaper than or equal to Bob's budget.\n");
		} else {
			printf("More than one collection is cheaper than or equal to Bob's budget or they are all more expensive.\n");
		}
return 0;
}