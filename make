// Saving working compilation command
g++ -std=c++17 -c main.cpp Application.cpp MenuState.cpp State.cpp StateStack.cpp TitleState.cpp LocalGameState.cpp GameServer.cpp OnlineGameState.cpp Utility.cpp && g++ *.o -o ../game -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network && rm *.o
