project: proj5.o Vehicle.o Car.o
	g++ -std=c++11 proj5.o Vehicle.o Car.o -o project

proj5.o: proj5.cpp Vehicle.o Car.o
	g++ -c -std=c++11 proj5.cpp -o proj5.o

Vehicle.o: Vehicle.cpp Vehicle.h
	g++ -c -std=c++11 Vehicle.cpp -o Vehicle.o

Car.o: Car.cpp Car.h Vehicle.h
	g++ -c -std=c++11 Car.cpp -o Car.o

clean:
	rm -f *.o
	rm project