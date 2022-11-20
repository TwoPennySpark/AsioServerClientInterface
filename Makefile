server:
	g++ -I $(shell pwd)/NetCommon/ main.cpp -o server --std=c++17 -pthread -lboost_system

client:
	g++ -I $(shell pwd)/NetCommon/ main.cpp -o client --std=c++17 -pthread -lboost_system -D CLIENT
