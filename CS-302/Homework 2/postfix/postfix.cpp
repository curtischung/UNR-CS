#include <iostream>
#include <string>

#include "StackLinked.h"

//forward declaration of the calculation function
float calculation(StackLinked<float> & stack, const std::string expression);

int main() {
//declaration of all postfix notations
	std::string str = "34+52/*";
	std::string str2 = "83+24-*";
	std::string str3 = "83*7+27/-";
	std::string str4 = "63/25-*";
	std::string str5 = "43*6+";
	std::string str6 = "52-63-*";

	StackLinked<float> object; //linkedlist class object
	
//testing each different postfix, calling the calculation function with apporpriate string and with a class object for push and pop functions
	std::cout << "1st: infix: (3 + 4) * (2 / 5), postfix: 34+52/*,  is equal to: " << calculation(object, str)<< std::endl;
	std::cout << "2nd: infix: (8 + 3) * (4 - 2), postfix: 34+52/*,  is equal to: " << calculation(object, str2) << std::endl;
	std::cout << "3rd: infix: (8 * 3) + 7 - (2 / 7), postfix: 83*7+27/-,  is equal to: " << calculation(object, str3) << std::endl;
	std::cout << "4th: infix: (6 / 3) * (2 - 5), postfix: 63/25-*, is equal to: " << calculation(object, str4) << std::endl;
	std::cout << "5th: infix: (4 * 3) + 6, postfix: 43*6+,  is equal to: " << calculation(object, str5) << std::endl;
	std::cout << "6th: infix: (5 - 2) * (6 - 3), postfix: 52-63-*,  is equal to: " << calculation(object, str6) << std::endl;
}

float calculation(StackLinked<float> & object, std::string expression){
	float result; //intialize a result 
	float operand1, operand2;
	for(size_t i = 0; i < expression.length(); i++) { //loop which iterates through the string until its max length
		if(expression[i] >= '0' && expression[i] <= '9') { //number check, if the string value is a number from 0 -9
			
			object.push(expression[i] - '0'); //it will push the number onto the stack

		} else if(expression[i] == '+') { //checks for addition operator on current value of the string
			
			operand1 = object.pop(); //pops the number values within the stack and returns them
			operand2 = object.pop();
			
			result = operand1 + operand2; //adds the numbers popped together
			object.push(result); //pushes the result to the stack

		} else if(expression[i] == '-') { //each of these functions work similar, corresponding to the operand sign
			operand1 = object.pop();
			operand2 = object.pop();
			
			result = operand1 - operand2;
			object.push(result);

		} else if(expression[i] == '*') {

			operand1 = object.pop();
			operand2 = object.pop();
			
			result = operand1 * operand2;
			object.push(result);

		} else if(expression[i] == '/') {

			operand1 = object.pop();
			operand2 = object.pop();
			
			result = operand1 / operand2;
			object.push(result);

		}
	}
	result = object.pop(); //sets the result to the final item in the stack(which should be the result of the postfix)
	return result;
}
