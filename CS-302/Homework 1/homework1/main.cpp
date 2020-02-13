#include "LinkedList.h"

int main() {
	int command = 0; //option controller
	LinkedList<int> object; //creates class object(list)
	int addItem;
	while(command != 5) { //checks for valid command number options
		std::cout << "Options: (code currently using int)" << std::endl
			 << "1. Add new item" << std::endl
			 << "2. Delete an item" << std::endl
			 << "3. Show number of items" << std::endl
			 << "4. Show all items" << std::endl
			 << "5. Exit" << std::endl
			 << "option: " << std::endl;
			 std::cin >> command;
			 if(command > 5) {
			 	std::cout << "Invalid." << std::endl; //if the option is larger than 5, returns invalid and asks to select another option
			 }
			 switch(command) {
			 	case 1: { //calls push(add function), which asks user to input an integer to add to list
			 		std::cout << "enter integer: ";
			 		std::cin >> addItem;
			 		object.push(addItem);
			 		std::cout << "List: " << object;
			 		break;
			 	}
			 	case 2:{ //delete option, pops the last element added
			 		object.pop();
			 		std::cout << "Popped: " << object;
			 		break;
			 	}
			 	case 3: { //returns total
			 		std::cout << "Total: " << object.total() << std::endl;
			 		break;
			 	}
			 	case 4: { //prints list to terminal
			 		std::cout << "All items on List: " << object;
			 	}
			 }
	}
}