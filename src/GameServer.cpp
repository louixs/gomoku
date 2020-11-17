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
, mGameStarted(false)
, mGameStartPlayerCount(2)
, mIsListening(false)
, mMaxConnectedPlayers(2)
, mConnectedPlayers(0)
, mPeers(1)
, mWaitingThreadEnd(false)

{
  mListenerSocket.setBlocking(false);
  mPeers[0].reset(new RemotePeer());
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
    //handleIncomingPackets();
    handleIncomingConnections();

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
    sf::sleep(sf::milliseconds(500));
  }
}

void GameServer::startGame() {
  mGameStarted = true;
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::GameStarted);
  packet << mGameStarted;
  sendToAll(packet);
}

void GameServer::tick(){
  // updateClientState();
  if ((mConnectedPlayers >= mGameStartPlayerCount) && !mGameStarted) {
    cout << "More than 2 players connect, game can be started" << endl;
    cout << "ConnectedPlayers: " << mConnectedPlayers << endl;
    cout << "gamestarted: " << mGameStarted << endl;
    startGame();
  }
}

void GameServer::broadcastMessage(const std::string& message)
{
  cout << "borad cast message: " << message << endl;
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

void GameServer::sendToAll(sf::Packet& packet) {
  for(PeerPtr& peer : mPeers) {
    if (peer->ready) {
      peer->socket.send(packet);
    }
  }
}

sf::Time GameServer::now() const {
  return mClock.getElapsedTime();
}

void GameServer::handleIncomingConnections(){
  if (!mIsListening) {
    return;
  }

  if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done) {
    cout << "player connected" << endl;

    sf::Packet packet;

    // send message
    // player connectd
    broadcastMessage("New player Connected!");

    mPeers[mConnectedPlayers]->ready = true;
    mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
    mConnectedPlayers++;

    if (mConnectedPlayers >= mMaxConnectedPlayers) {
      cout << "Reached max number of players. Stop listening." << endl;
      setListening(false);

    } else { // add a new waiting peer
      mPeers.push_back(PeerPtr(new RemotePeer()));
    }
  }
}
