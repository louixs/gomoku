// Saving working compilation command
g++ -std=c++17 -c main.cpp LocalGame.cpp GameServer.cpp NetworkedMultiplayerGame.cpp && g++ GameServer.o main.o -o ../game -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network && rm *.o
