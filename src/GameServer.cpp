#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include "GameUtility.hpp"
#include <iostream>
#include <SFML/Network/Packet.hpp>
#include "Globals.hpp"

using namespace std;

GameServer::RemotePeer::RemotePeer()
: ready(false)
, timedOut(false)
{
  socket.setBlocking(false);
}

GameServer::GameServer()
: mThread(&GameServer::executionThread, this)
, mBoard(g_boardSize, std::vector<int>(g_boardSize, 0))
, mClientTimeoutTime(sf::seconds(60.f))
, mCurrentTurn(Game::First)
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
    handleIncomingPackets();
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

void GameServer::updateClientState() {
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::UpdateClientState);
  packet << static_cast<sf::Int32>(mCurrentTurn);
  sendToAll(packet);
}

void GameServer::startGame() {
  mGameStarted = true;
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::GameStarted);
  packet << mGameStarted;
  sendToAll(packet);
}


void GameServer::tick(){
  //updateClientState();

  if ((mConnectedPlayers >= mGameStartPlayerCount) && !mGameStarted) {
    broadcastMessage("All players have connected. Game starts now!");
    startGame();
  }
}

void GameServer::broadcastMessage(const std::string& message)
{
  cout << "broadcast message: " << message << endl;
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

  bool detectedTimeout = false;

  for(PeerPtr& peer : mPeers) {
    if (peer->ready) {
      sf::Packet packet;
      while (peer->socket.receive(packet) == sf::Socket::Done) {
        handleIncomingPacket(packet, *peer, detectedTimeout);
        peer->lastPacketTime = now();
        packet.clear();
      }

      if (now() >= peer->lastPacketTime + mClientTimeoutTime) {
        cout << "handleIncomingPackets" << endl;
        cout << now().asMilliseconds() << endl;
        cout << peer->lastPacketTime.asMilliseconds() << endl;
        cout << mClientTimeoutTime.asMilliseconds() << endl;
        cout << "handleIncomingPackets: dtected timeout" << endl;
        peer->timedOut = true;
        detectedTimeout = true;
      }
    }
  }

  if (detectedTimeout) {
    handleDisconnections();
  }
}

void GameServer::handleDisconnections() {
  for (auto itr = mPeers.begin(); itr != mPeers.end(); ) {
    if ((*itr)->timedOut) {

      mConnectedPlayers--;
      itr  = mPeers.erase(itr);

      // Go back to a listening state if needed
      if (mConnectedPlayers < mMaxConnectedPlayers) {
        mPeers.push_back(PeerPtr(new RemotePeer()));
        setListening(true);
      }

      broadcastMessage("A player has disconnected");
    }
    else {
      ++itr;
    }
  }
}

void GameServer::changeTurn () {
  if (mCurrentTurn == Game::First) {
    mCurrentTurn = Game::Second;
  } else {
    mCurrentTurn = Game::First;
  }
}

void GameServer::sendTurnUpdate () {
  cout << "Send turn update" << endl;
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::TurnUpdate);
  packet << mCurrentTurn;
  sendToAll(packet);
}

void GameServer::sendWinner (const Game::Turns& turn) {
  cout << "Send winner!" << endl;
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Server::WinnerUpdate);
  packet << turn;
  sendToAll(packet);
}

void GameServer::handleIncomingPacket(sf::Packet& packet,
                                      RemotePeer& receivingPeer,
                                      bool& detectedTimeout) {
  sf::Int32 packetType;
  packet >> packetType;

  switch (packetType) {
    case Client::Quip: {
      cout << "Received Client::Quip" << endl;

      sf::Int32 effect;
      packet >> effect;

      {
        sf::Packet packet;
        packet << static_cast<sf::Int32>(Server::Quip);
        packet << effect;
        sendToAll(packet);
      }
    } break;

    case Client::Quit: {
      cout << "Received Client::Quit" << endl;
      receivingPeer.timedOut = true;
      detectedTimeout = true;
    } break;

    case Client::PositionUpdate: { // improve naming
      int x;
      int y;

      packet >> x;
      packet >> y;
      cout << "position update received!" << endl;
      cout << "x" << x << endl;
      cout << "y" << y << endl;

      // Update clients with new stone positions
      {
        cout << "Sending position update" << endl;
        sf::Packet packet;
        packet << static_cast<sf::Int32>(Server::PositionUpdate);
        packet << x;
        packet << y;
        sendToAll(packet);
      }

      // update server side board
      mBoard[x][y] = mCurrentTurn;

      // check if anyone has won
      if (GameUtility::hasWon(mBoard, g_boardSize, mWinStoneCount, mCurrentTurn, x, y)) {
        // winner is the one that has played at current turn
        sendWinner(mCurrentTurn);
        
      } else {
        // otherwise change turn and send the new turn to clients
        changeTurn();
        sendTurnUpdate();
      }

    } break;
  }
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
    // assign player turn
    int playerTurn = mConnectedPlayers + 1;
    // send message
    // player connectd
    broadcastMessage("New player connected!");
    // assign stone
    mPeers[mConnectedPlayers]->turn = playerTurn;
    mPeers[mConnectedPlayers]->ready = true;
    mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts

    // send user data to client
    packet << static_cast<sf::Int32>(Server::PlayerData);
    packet << playerTurn;
    mPeers[mConnectedPlayers]->socket.send(packet);

    mConnectedPlayers++;

    if (mConnectedPlayers >= mMaxConnectedPlayers) {
      cout << "Reached max number of players. Stop listening." << endl;
      setListening(false);

    } else { // add a new waiting peer
      mPeers.push_back(PeerPtr(new RemotePeer()));
    }
  }
}
