//Circle.c Project 1 by Curtis Chung, Lab Section 1103

#include <stdio.h>

#define PI 3.14, 

int main (void)
{
	int rad;
	int diameter, squareperimeter, squarearea;
	float circum, circlearea, difference;

		printf("Enter the value of the radius: ");
		scanf("%d", &rad);

			diameter = 2 * rad;
			circum = 2 * PI * rad;
			circlearea = PI * (rad * rad);
			squareperimeter = (2*rad) + (2*rad) + (2*rad) + (2*rad);
			squarearea = (2*rad) * (2*rad);
			difference = squarearea - circlearea;
		printf("The diamater of the circle is: %d\n", diameter);

		printf("The circumference of the circle is: %0.3f\n", circum);

		printf("The area of the circle is: %0.3f\n", circlearea);

		printf("The perimeter of the square is: %d\n", squareperimeter);

		printf("The area of the square is: %d\n", squarearea);

		printf("The difference between the area of the square and the circle is: %.3f\n", difference);



	return 0;
}