objects=main.o Train.o Test.o
nckrl: $(objects)
	g++ -std=c++11 -o nckrl $(objects)
main.o: main.cpp
	g++ -std=c++11 -c main.cpp
Train.o: Train.cpp
	g++ -std=c++11 -c Train.cpp
Test.o: Test.cpp
	g++ -std=c++11 -c Test.cpp
clean:
	rm nckrl $(objects)