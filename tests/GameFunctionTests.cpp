#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>

#include "../src/GameUtility.hpp"

const int boardSize = 19;
const int requiredStonesToWin = 5;
const int playerTurn = 1;

TEST_CASE("Test isLegal") {
  // init 2d vector board
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));
 
  board[1][1] = 1; // place stone, this will be an illegal place to place


  REQUIRE(isLegal(board, 0, 0) == true);
  REQUIRE(isLegal(board, 1, 1) == false);
}

TEST_CASE("Test getGameEndStr when player has won") {
  int currentTurn = 1;
  int playerTurn = 1;

  REQUIRE(getGameEndStr(currentTurn, playerTurn) == "You've won!");
}

TEST_CASE("Test getGameEndStr when player has lost") {
  int currentTurn = 2;
  int playerTurn = 1;

  REQUIRE(getGameEndStr(currentTurn, playerTurn) == "You've lost!");
}

TEST_CASE("Test hasWon function - vertical case") {
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));
  
  // vertical win check
  for (int i = 0; i < 6; i++) {
    board[0][i] = playerTurn;
  }

  for (int i = 0; i < 6; i++) {
    REQUIRE(hasWon(board, boardSize, requiredStonesToWin, playerTurn, 0, i) == true);
  }

}

TEST_CASE("Test hasWon function - horizontal case") {
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));

  for (int i = 0; i < 6; i++) {
    board[i][0] = playerTurn;
  }

  for (int i = 0; i < 6; i++) {
    REQUIRE(hasWon(board, boardSize, requiredStonesToWin, playerTurn, i, 0) == true);
  }
}

TEST_CASE("Test hasWon function - diagonal right up/down case") {
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));

  int startingPoint = 5;
  int endPoint = startingPoint + requiredStonesToWin;
  
  {
    int y = startingPoint;
    for (int x = startingPoint; x <= endPoint; x++, y--) {
      board[x][y] = playerTurn;
    }
  }

  {
    int y = startingPoint;
    for (int x = startingPoint; x <= endPoint; x++, y--) {
      REQUIRE(hasWon(board, boardSize, requiredStonesToWin, playerTurn, x, y) == true);
    }
  }
}

TEST_CASE("Test hasWon function - diagonal left up/down case") {
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));

  int startingPoint = 5;
  int endPoint = startingPoint - requiredStonesToWin;

  {
    int y = startingPoint;
    for (int x = startingPoint; x >= endPoint; x--, y--) {
      board[x][y] = playerTurn;
    }
  }

  {
    int y = startingPoint;
    for (int x = startingPoint; x >= endPoint; x--, y--) {
      REQUIRE(hasWon(board, boardSize, requiredStonesToWin, playerTurn, x, y) == true);
    }
  }
}

TEST_CASE("Test hasWon function - false case") {
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));

  board[0][5] = 0;
  REQUIRE(hasWon(board, boardSize, requiredStonesToWin, playerTurn, 0, 5) == false);

}
