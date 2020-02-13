//--------------------------------------------------------------------
//
//   delimiters.cpp
//
//--------------------------------------------------------------------

#include <iostream>


#include "StackLinked.h"


//--------------------------------------------------------------------

bool delimitersOk ( const string &expression ) throw (logic_error);

//--------------------------------------------------------------------

int main()
{
    string inputLine;            // Input line
//    char   ch;                   // Holding pen for input chars

    cout << "This program checks for properly matched delimiters."
         << endl;

    //the 5 test strings
    string str1 = "(([<>]))";
    string str2 = "(This should be invalid";
    string str3 = "{{)";
    string str4 = "<>(){}[]";
    string str5 = "[This should be valid]";
    /*while( cin )
    {
        cout << "Enter delimited expression (<EOF> to quit) : "
             << endl;

        // Read in one line
        inputLine = "";
        cin.get(ch);
        while( cin && ch != '\n' )
        {
            inputLine = inputLine + ch;
            cin.get(ch);
        }

        if( ! cin )              // Reached EOF: stop processing
            break;
    
        if ( delimitersOk (inputLine) )
            cout << "Valid" << endl;
        else
            cout << "Invalid" << endl;
    }*/

    //just calls the function of delimetersOK and prints the output as valid or invald
    std::cout << "Test 1: " << str1 << " is: " << (delimitersOk(str1) ? "Valid" : "Invalid") << std::endl;
    std::cout << "Test 2: " << str2 << " is: " << (delimitersOk(str2) ? "Valid" : "Invalid") << std::endl;
    std::cout << "Test 3: " << str3 << " is: " << (delimitersOk(str3) ? "Valid" : "Invalid") << std::endl;
    std::cout << "Test 4: " << str4 << " is: " << (delimitersOk(str4) ? "Valid" : "Invalid") << std::endl;
    std::cout << "Test 5: " << str5 << " is: " << (delimitersOk(str5) ? "Valid" : "Invalid") << std::endl;
    return 0;
}

//--------------------------------------------------------------------
// delimitersOk: the function that students must implement for 
//    Programming Exercise 3.
// Note: we used the term "braces" to describe '[' and ']'. It would
//    have been better to use the term "brackets". The program can be
//    easily modified to process any set of matching delimiters.
//--------------------------------------------------------------------

// Insert your delimitersOk function below

bool delimitersOk ( const string &expression ) throw (logic_error){
    StackLinked<char> object; //class object
    char check; //char object to hold the popped value

try {
    for(size_t i = 0; i < expression.length(); i++) { //iterates through the string
        if(expression[i] == '(' || expression[i] == '{' || expression[i] == '<' || expression[i] == '[') { //checks for the beginning bracket notations
            
            object.push(expression[i]); //pushes opening bracket notation onto stack
        
        } else if (expression[i] == ')') { //checks closing bracket
            check = object.pop(); //pops stack opening bracket (
            if(check != '(') { //checks for a matching opening parentheses
                return false; //if no matching, returns false
            }
        //all other if statements function similarly, interchanging between different kinds of brackets    
        } else if (expression[i] == '}') {
            check = object.pop();
            if(check != '{') {
                return false;
            }
        } else if (expression[i] == '>') {
            check = object.pop();
            if(check != '<') {
                return false;
            }
        } else if (expression[i] == ']') {
            check = object.pop();
            if(check != '[') {
                return false;
            }
        }
    }
    return object.isEmpty(); //if each close bracket has a matching open, 
                            //stack should be empty, so the string bracket notation should be true
    } catch (logic_error) { //if there is an invalid bracket, exception should return false
        return false;
    }
}   
