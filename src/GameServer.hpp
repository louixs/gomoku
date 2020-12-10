#ifndef __GAMESERVER_H_
#define __GAMESERVER_H_

#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>

#include "GameStateIdentifiers.hpp"

class GameServer {
  public:
    GameServer();
    ~GameServer();

  private:
    static const int mBoardSize = 19;
    static const int mWinStoneCount = 5;
    std::vector<std::vector<int> > mBoard;
    sf::Thread mThread;
    sf::Clock mClock;
    bool mWaitingThreadEnd;
    sf::TcpListener mListenerSocket;
    bool mIsListening;
    sf::Time mClientTimeoutTime;
    std::size_t mMaxConnectedPlayers;
    std::size_t mConnectedPlayers;
    bool mGameStarted;
    const unsigned long mGameStartPlayerCount;
    Game::Turns mCurrentTurn;
    
    struct RemotePeer {
      RemotePeer();
      sf::TcpSocket socket;
      sf::Time lastPacketTime;
      bool ready;
      bool timedOut;
      int turn;
    };
    // Unique pointer to remote peers
    typedef std::unique_ptr<RemotePeer> PeerPtr;
    std::vector<PeerPtr> mPeers;

  private:
    void executionThread();
    void tick();
    void handleIncomingPackets();
    void handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);
    void handleIncomingConnections();
    void handleDisconnections();
    void setListening(bool enable);
    void broadcastMessage(const std::string& message);
    void sendToAll(sf::Packet& packet);
    void startGame();
    void assignTurns();
    void updateClientState();
    void changeTurn();
    void sendTurnUpdate();
    void sendWinner(const Game::Turns& turn);
    sf::Time now() const;
};

#endif // __GAMESERVER_H_
