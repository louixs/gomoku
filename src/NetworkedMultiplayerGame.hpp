#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
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
    TextureHolder mTextures;
    sf::Sprite mBlackStone;
    sf::Sprite mWhiteStone;
    FontHolder mFonts;
    int mWinner;
    sf::Text mInfoText;
    std::string winnerStr;
    sf::ContextSettings mSettings;
    sf::RenderWindow mWindow;
    const sf::Time TimePerFrame;
    bool mGameStarted;
    turns mPlayerTurn;
        
  // for networking
  private:
    bool mIsHost;
    std::unique_ptr<GameServer> mGameServer;
    std::vector<std::string> mBroadcasts;
    sf::Text mBroadcastText;
    sf::Time mBroadcastElapsedTime;
    bool mConnected;
    sf::TcpSocket mSocket;
    bool mIsTurn;

  private:
    void loadTextures();
    void initStones();
    inline bool isLegal(int x, int y);
    void drawBoard();
    void drawStones();
    void drawWinnerText();
    std::string getWinnerStr(int stone);
    bool hasWon(int x, int y);
    void handleInput(sf::Event event);
    void processEvents();
    void update(sf::Time dt);
    void render();

    // networking
    void handlePacket(sf::Int32 packetType, sf::Packet& packet);
    void updateBroadcastMessage(sf::Time elapsedTime);
    void drawBroadcast();
    void sendPositionUpdates(int x, int y);
};



#endif // __NETWORKEDMULTIPLAYERGAME_H_
