#ifndef __GAMEUTILITY_H_
#define __GAMEUTILITY_H_

#include <functional>
#include <vector>

bool isLegal(std::vector<std::vector<int> > board, int x, int y) {
  return board[x][y] == 0 || false;
}

#endif // __GAMEUTILITY_H_
