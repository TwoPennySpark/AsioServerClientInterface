server:
	g++ main.cpp -o server --std=c++17 -pthread -lboost_system

client:
	g++ main.cpp -o client --std=c++17 -pthread -lboost_system -D CLIENT
