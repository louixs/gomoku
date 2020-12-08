#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../src/GameUtility.hpp"

TEST_CASE("Test isLegal") {
  const int boardSize = 19;
  // init 2d vector board
  std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, 0));
 
  board[1][1] = 1; // place stone, this will be an illegal place to place


  REQUIRE(isLegal(board, 0, 0) == true);
  REQUIRE(isLegal(board, 1, 1) == false);
}
