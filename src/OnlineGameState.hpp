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
    std::vector<std::vector<int> > mBoard;
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
    inline bool isMyTurn();
    void handleMouseInput(const sf::Event& event);
    void handleKeyInput(const sf::Event& event);
    void drawBoard(sf::RenderWindow& window);
    void drawStones(sf::RenderWindow& window);
    void drawBroadcast(sf::RenderWindow& window);
    void drawInfoText(sf::RenderWindow& window);
    void handleQuips(const sf::Event& event);
    void goToMenu(const sf::Event& event);
    void playStoneClick();

    // networking
    void handlePacket(sf::Int32 packetType, sf::Packet& packet);
    void updateBroadcastMessage(sf::Time elapsedTime);
    void sendPositionUpdates(int x, int y);
    void sendQuip(SoundEffect::ID id);

};



#endif // __ONLINEGAMMESTATE_H_
