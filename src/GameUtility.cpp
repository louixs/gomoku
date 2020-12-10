#include "GameUtility.hpp"

bool GameUtility::isLegal(std::vector<std::vector<int> >& board, int x, int y) {
    return board[x][y] == 0 || false;
  }

// TODO: refactor!
bool GameUtility::hasWon(std::vector<std::vector<int> >& board, int boardSize, int winCount, int currentTurn, int x, int y) {

  int total_count = 0;
  int current_color = 0;

    // VERTICAL check
    // count up
    int y1 = y;
    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x][y1];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      y1--;
    } while (y1 > 0 && current_color == currentTurn);

    // keep checking if the function hasn't exited
    // count down
    // reset params
    y1 = y;
    // avoid double count
    total_count = total_count - 1;

    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x][y1];
      if (current_color == currentTurn) {
        total_count++;
      }
      y1++;

    } while (y1 < boardSize && current_color == currentTurn);

    // Horizontal check
    // reset params
    int x1 = x;
    total_count = 0;

    // check left
    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1--;
    } while (x1 > 0 && current_color == currentTurn);

    // check right
    // reset vars
    x1 = x;

    // avoid double count
    total_count = total_count - 1;

    // check right
    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1++;
    } while (x1 < boardSize && current_color == currentTurn);

    // DIAGONAL

    // reset vars
    x1 = x;
    y1 = y;
    total_count = 0;

    // 1. LEFT UP RIGHT DOWN
    // check left up
    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y1];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1--;
      y1--;
    } while (x1 > 0 && y1 > 0 && current_color == currentTurn);

    // check right down
    // reset vars
    x1 = x;
    y1 = y;

    // avoid double count
    total_count = total_count - 1;

    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y1];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1++;
      y1++;
    } while (x1 < boardSize && y < boardSize && current_color == currentTurn);


    // 2. LEFT TO RIGHT UP
    // reset vars
    x1 = x;
    y1 = y;
    total_count = 0;

    // check left down
    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y1];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1--;
      y1++;
    } while (x1 > 0 && y < boardSize && current_color == currentTurn);

    // right up
    // reset vars
    x1 = x;
    y1 = y;

    total_count = total_count - 1;

    do {
      if (total_count == winCount) {
        return true;
      }

      current_color = board[x1][y1];
      // if the current color matches the color to check
      // increment the total count
      if (current_color == currentTurn) {
        total_count++;
      }

      x1++;
      y1--;
    } while (x1 < boardSize && y > 0 && current_color == currentTurn);

    return false;
  };


std::string GameUtility::getGameEndStr(int currentTurn, int playerTurn) {
    if (currentTurn == playerTurn) {
      return "You've won!";
    } else {
      return "You've lost!";
    }
}
