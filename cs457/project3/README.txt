Written by: Curtis Chung
Date: 3/11/20
Project 2 for CS 457

///// Compilation Instructions /////

g++ main.cpp
./a.out

////////////////////////////////////

///// Running Program /////

Please use the PA2_test.sql I have modified in my submission.
I have gotten rid of all the spacing, as well as made the update
commands into one full line.
Use this by please copy/paste the test file into the terminal.

I will also be providing a screen shot of my results using
this exact method.

//////////////////////////

///// DOCUMENTATION //////

//////////////////////////

1 - How your program store tuples in the table

My program stores tuples inside the table created through a 2-D vector inside
the table class called vectorData. The tuple vectors are pushed onto the datavector
in the specific row/column within the dataVector according to the input of the
user. 


2 - At a very high level, how you implement those required functionalities, 
	i.e., tuple insertion, deletion, modification, and query.

For the tuple insertion, I created a string of newItems, searched for where the new
items that needed to be inserted started by searching for the beginning "(" and pushed
the new data onto the dataVector. 

For the deletion of the tuples, I searched for the key words of the "where" statement inside
the command given by the user using a loop, as well as searched for the specific operating conditions (=, <, >), and if the conditions were met, they were removed from the vectorData using vector.erase inside my delete method.

Tuple modification (update) had a similar implementation to the delete, finding the key words "where" and "set" and took the values for each of those statements and replaced those specific area values with the new values.

Table query, once again, searches for the "where" statement and loops through the table to find the statement where the condition is satisified. Once it is satisified, the values will be printed to the terminal.
