#ifndef __GAMESERVER_H_
#define __GAMESERVER_H_

#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>

#define BLACK (1)
#define WHITE (2)

class GameServer {
  public:
    GameServer();
    ~GameServer();

  private:
    static const int mBoardSize = 19;
    int mBoard[mBoardSize][mBoardSize] = { 0 };
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
    enum mTurns { FIRST = BLACK, SECOND = WHITE };
    mTurns mCurrentTurn;
    
    struct RemotePeer {
      RemotePeer();
      sf::TcpSocket socket;
      sf::Time lastPacketTime;
      bool ready;
      bool timedOut;
      int turn;
      //std::string name;
      // coord - where the stone is placed
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
    bool hasWon(int x, int y);
    void assignTurns();
    void updateClientState();
    void changeTurn();
    void sendTurnUpdate();
    void sendWinner(const mTurns& currentTurn);
    sf::Time now() const;
};

#endif // __GAMESERVER_H_
