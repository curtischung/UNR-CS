project: proj7.o MyString.o
	g++ -std=c++11 proj7.o MyString.o -o project

proj7.o: proj7.cpp MyString.o
	g++ -c -std=c++11 proj7.cpp -o proj7.o

MyString.o: MyString.cpp MyString.h
	g++ -c -std=c++11 MyString.cpp -o MyString.o

clean:
	rm -f *.o
	rm project