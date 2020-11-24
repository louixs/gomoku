#ifndef __LOCALGAMESTATE_H_
#define __LOCALGAMESTATE_H_

#include "State.hpp"
#include <SFML/Graphics.hpp>

#define BLACK (1)
#define WHITE (2)

class LocalGameState : public State {
  public:
    LocalGameState();

    LocalGameState(StateStack& stack, Context context);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);

  private:
    int mCellSize;
    static const int mBoardSize = 19;
    int mBoard[mBoardSize][mBoardSize] = { 0 };
    enum turns { FIRST = BLACK, SECOND = WHITE };
    turns mCurrentTurn;
    sf::Sprite mBlackStone;
    sf::Sprite mWhiteStone;
    sf::Text mInfoText;
    std::string winnerStr;
    sf::ContextSettings mSettings;

  private:
    inline bool isLegal(int x, int y);
    inline void changeTurn();
    void drawBoard(sf::RenderWindow& window);
    void drawStones(sf::RenderWindow& window);
    void drawWinnerText(sf::RenderWindow& window);
    std::string getWinnerStr(int stone);
    bool hasWon(int x, int y);
};


#endif // __LOCALGAMESTATE_H_
