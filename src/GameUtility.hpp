#ifndef __GAMEUTILITY_H_
#define __GAMEUTILITY_H_

#include <vector>
#include <string>

namespace GameUtility {

  bool isLegal(std::vector<std::vector<int> >& board, int x, int y);

  bool hasWon(std::vector<std::vector<int> >& board, int boardSize, int winCount, int currentTurn, int x, int y);

  std::string getGameEndStr(int currentTurn, int playerTurn);
};

#endif // __GAMEUTILITY_H_
