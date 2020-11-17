#include <SFML/Graphics.hpp>
#include <string>
#ifndef __GAME_H_
#define __GAME_H_

#define BLACK (1)
#define WHITE (2)

class LocalGame {
  public:
    LocalGame();
    void run();

  private:
    int mCellSize;
    static const int mBoardSize = 19;
    int mBoard[mBoardSize][mBoardSize] = { 0 };
    enum turns { FIRST = BLACK, SECOND = WHITE };
    turns mCurrentTurn;
    sf::Texture mBlackStoneTexture;
    sf::Texture mWhiteStoneTexture;
    sf::Sprite mBlackStone;
    sf::Sprite mWhiteStone;
    sf::Font mMainFont;
    sf::Text mWinnerText;
    std::string winnerStr;
    sf::ContextSettings mSettings;
    sf::RenderWindow mWindow;

  private:
    inline bool isLegal(int x, int y);
    inline void changeTurn();
    void drawBoard();
    void drawStones();
    void drawWinnerText();
    std::string getWinnerStr(int stone);
    bool hasWon(int x, int y);
    void processEvents();
    void update();
    void render();
};

#endif // __GAME_H_
