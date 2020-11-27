#ifndef __ONLINEGAMMESTATE_H_
#define __ONLINEGAMMESTATE_H_

#include "State.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "GameStateIdentifiers.hpp"
#include "NetworkProtocol.hpp"
#include "GameServer.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>

#include <string>
#include <vector>

class OnlineGameState : public State {
  public:
    OnlineGameState(StateStack& stack, Context context, bool isHost);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);
    virtual void onActivate();
    void onDestroy();

  private:
    sf::RenderWindow& mWindow;
    int mCellSize;
    static const int mBoardSize = 19;
    int mBoard[mBoardSize][mBoardSize] = { 0 };
    Game::Turns mCurrentTurn;
    sf::Sprite mBlackStone;
    sf::Sprite mWhiteStone;
    int mWinner;
    sf::Text mInfoText;
    std::string winnerStr;
    bool mGameStarted;
    Game::Turns mPlayerTurn;

  // for networking
  private:
    bool mIsHost;
    std::unique_ptr<GameServer> mGameServer;
    std::vector<std::string> mBroadcasts;
    sf::Text mBroadcastText;
    sf::Time mBroadcastElapsedTime;
    bool mConnected;
    bool mActiveState;
    sf::TcpSocket mSocket;
    sf::Text mFailedConnectionText;
    SoundPlayer& mSounds;
    sf::Clock mFailedConnectionClock;
    sf::Time mClientTimeout;
	sf::Time mTimeSinceLastPacket;
    bool mIsTurn;

  private:    
    inline bool isLegal(int x, int y);
    inline bool isMyTurn();
    void handleInput(const sf::Event& event);
    void drawBoard(sf::RenderWindow& window);
    void drawStones(sf::RenderWindow& window);
    void drawWinnerText(sf::RenderWindow& window);
    void drawBroadcast(sf::RenderWindow& window);
    std::string getWinnerStr(Game::Turns& turn);
    bool hasWon(int x, int y);
    void sendQuip(const sf::Event& event);
    void playStoneClick();

    // networking
    void handlePacket(sf::Int32 packetType, sf::Packet& packet);
    void updateBroadcastMessage(sf::Time elapsedTime);
    void sendPositionUpdates(int x, int y);
};


#endif // __ONLINEGAMMESTATE_H_
