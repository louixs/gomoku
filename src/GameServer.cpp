#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include <iostream>
#include <SFML/Network/Packet.hpp>

using namespace std;

GameServer::RemotePeer::RemotePeer()
: ready(false)
, timedOut(false)
{
  socket.setBlocking(false);
}

GameServer::GameServer()
: mThread(&GameServer::executionThread, this)
, mIsListening(false)
, mMaxConnectedPlayers(2)
, mConnectedPlayers(0)
, mPeers(1)
, mWaitingThreadEnd(false)

{
  mListenerSocket.setBlocking(false);
  mThread.launch();
}

GameServer::~GameServer() {
  mWaitingThreadEnd = true;
  mThread.wait();
}

void GameServer::setListening(bool enable) {
  // check if it isn't already listening
  if (enable) {
    if (!mIsListening) {
      mIsListening = (mListenerSocket.listen(ServerPort) == sf::TcpListener::Done);
    } else {
      mListenerSocket.close();
      mIsListening = false;
    }
  }
}

void GameServer::executionThread() {
  setListening(true);

  // stepInterval updates the state frequently
  // so the server has the latest game state
  sf::Time stepInterval = sf::seconds(1.f / 60.f);
  sf::Time stepTime = sf::Time::Zero;

  // tick is used to update connected clients
  // involving networking so runs less frequently
  // to avoid sending too many updates to clog network
  sf::Time tickInterval = sf::seconds(1.f / 20.f);
  sf::Time tickTime = sf::Time::Zero;
  sf::Clock stepClock, tickClock;

  while (!mWaitingThreadEnd) {
    handleIncomingPackets();

    stepTime += stepClock.getElapsedTime();
    stepClock.restart();

    tickTime += tickClock.getElapsedTime();
    tickClock.restart();

    while (stepTime >= stepInterval) {
      // do something
      stepTime -= stepInterval;
    }

    while (tickTime >= tickInterval) {
      tick();
      tickTime -= tickInterval;
    }

    // Sleep to prevent server from consuming 100% CPU
    sf::sleep(sf::milliseconds(100));
  }
}

void GameServer::tick(){
  // updateClientState();
}

void GameServer::broadcastMessage(const std::string& message)
{
  for (std::size_t i = 0; i < mConnectedPlayers; ++i)
  {
    if (mPeers[i]->ready)
    {
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Server::BroadcastMessage);
      packet << message;

      mPeers[i]->socket.send(packet);
    }
  }
}

void GameServer::handleIncomingPackets(){
}

void GameServer::handleIncomingPacket(){

}

sf::Time GameServer::now() const {
  return mClock.getElapsedTime();
}

void GameServer::handleIncomingConnections(){
  if (!mIsListening) {
    return;
  }

  if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done) {
    sf::Packet packet;

    // send message
    // player connectd
    broadcastMessage("New player Connected!");

    mPeers[mConnectedPlayers]->ready = true;
    mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
    mConnectedPlayers++;

    if (mConnectedPlayers >= mMaxConnectedPlayers) {
      setListening(false);
    } else { // add a new waiting peer
      mPeers.push_back(PeerPtr(new RemotePeer()));
    }
  }
}
