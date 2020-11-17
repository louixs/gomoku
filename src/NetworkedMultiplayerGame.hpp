#include "NetworkProtocol.hpp"
#include "GameServer.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include "GameServer.hpp"
#include <string>
#include <vector>

#ifndef __NETWORKEDMULTIPLAYERGAME_H_
#define __NETWORKEDMULTIPLAYERGAME_H_

#define BLACK (1)
#define WHITE (2)

class NetworkedMultiplayerGame {
  public:
    NetworkedMultiplayerGame(bool isHost);
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
    bool mGameStarted;
    bool mIsHost;

  // for networking
  private:
    std::unique_ptr<GameServer> mGameServer;
    std::vector<std::string> mBroadcasts;
    sf::Text mBroadcastText;
    sf::Time mBroadcastElapsedTime;
    bool mConnected;
    sf::TcpSocket mSocket;

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



#endif // __NETWORKEDMULTIPLAYERGAME_H_
