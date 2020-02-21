Written by : Curtis Chung
Date : 2/20/2020
Project 1 for CS 457

///// Compilation Instructions /////

g++ main.cpp
./a.out

NOTE** When using the PA1_test.sql, do not use the actual file to test the commands.
Please copy/paste the test file into the terminal.

I will also provide a screenshot of my results in my terminal using the commands provided in the test script.


////////////////////////////////////

////////////////////////////////////
           DOCUMENTATION
////////////////////////////////////

1. How your program organizes multiple databases.

My database is organized by the system call function, which everytimea database is created, it creates a new folder under the name of which the user has inputed for the name of the databasw.

2. How your program manages multiple tables.

My program ensures that each table has a database to be written to as a .txt file inside the database folder. If a database is not selected, the program will not create a table as it has no destination.

3. AT a very high level, how did I implement those required functionalities?

The creation of the tables and databases are all done through the system calls, constant statements checking for a specific command put in, and if that command string is found, the program will do a system call to the corresponding command.

When a table is created, it is created as a vector object that holds the parameters given to the table when created by the user. The vector stores the data given to the table such as the database associated with that table, the parameters given, in this case, were strings, and the name of the table itself. The program also ensured that the tables were made in the selected database, and if it were not selected, then the tables would not be created.

In order to check the names of the databases and tables created, I implemented them using the library <sys/stat.h>, which takes the name of the string passed through, in most cases the name of the table/db, and checks if the file exists. If it returns 0, that means the filename does not exist in the directory. In order to check the table names, I had to ensure to make a new string which accounted for which database the table was stored in.