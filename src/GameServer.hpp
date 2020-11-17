#ifndef __GAMESERVER_H_
#define __GAMESERVER_H_

#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>

class GameServer {
public:
  GameServer();
  ~GameServer();
private:
  void executionThread();
  void tick();
  void handleIncomingPackets();
  void handleIncomingPacket();
  void handleIncomingConnections();
  void setListening(bool enable);
  void broadcastMessage(const std::string& message);
  void sendToAll(sf::Packet& packet);
  void startGame();
  void assignTurns();
  sf::Time now() const;

private:
  sf::Thread mThread;
  sf::Clock mClock;
  bool mWaitingThreadEnd;
  sf::TcpListener mListenerSocket;
  bool mIsListening;
  std::size_t mMaxConnectedPlayers;
  std::size_t mConnectedPlayers;
  bool mGameStarted;
  const unsigned long mGameStartPlayerCount;

  struct RemotePeer {
    RemotePeer();
    sf::TcpSocket socket;
    sf::Time lastPacketTime;
    bool ready;
    bool timedOut;
    // stone color
    // coord - where the stone is placed
  };
  // Unique pointer to remote peers
  typedef std::unique_ptr<RemotePeer> PeerPtr;
  std::vector<PeerPtr> mPeers;
};

#endif // __GAMESERVER_H_
