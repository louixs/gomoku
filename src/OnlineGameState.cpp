#include "OnlineGameState.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Network/IpAddress.hpp>

#include <iostream>
#include <fstream>

using namespace std;

sf::IpAddress getAddressFromFile()
{
  { // Try to open existing file
    // modify the ip address if connecting
    // to an arbitrary remote IP
    std::ifstream inputFile("ip.txt");
    std::string ipAddress;
    if (inputFile >> ipAddress)
      return ipAddress;
  }

  // If open/read failed, create new file
  std::ofstream outputFile("ip.txt");
  std::string localAddress = "127.0.0.1";
  outputFile << localAddress;
  return localAddress;
}

OnlineGameState::OnlineGameState(StateStack& stack, Context context, bool isHost)
  : State(stack, context)
  , mWindow(*context.window)
  , mCellSize(40)
  , mCurrentTurn(Game::First)
  , mConnected(false)
  , mGameServer(nullptr)
  , mActiveState(true)
  , mIsHost(isHost)
  , mGameStarted(false)
  , mClientTimeout(sf::seconds(60.f))
  , mTimeSinceLastPacket(sf::seconds(0.f))
  , mIsTurn(false)
  , mWinner(0)
  , mSounds(*context.sounds)
{
  context.music->stop();
  sf::Font& font = context.fonts->get(Fonts::Main);
  sf::Texture& blackStoneTexture = context.textures->get(Textures::BlackStone);
  sf::Texture& whiteStoneTexture = context.textures->get(Textures::WhiteStone);

  blackStoneTexture.setSmooth(true);
  whiteStoneTexture.setSmooth(true);

  // set stone sprites
  mBlackStone.setTexture(blackStoneTexture);
  mWhiteStone.setTexture(whiteStoneTexture);

  // set stone scales
  mBlackStone.setScale(1.0*mCellSize / mBlackStone.getLocalBounds().width,
                       1.0*mCellSize / mBlackStone.getLocalBounds().height);
  mWhiteStone.setScale(1.0*mCellSize / mWhiteStone.getLocalBounds().width,
                       1.0*mCellSize / mWhiteStone.getLocalBounds().height);


  // init winner text
  mInfoText.setFont(font);
  mInfoText.setPosition(5.f, 5.f);
  mInfoText.setCharacterSize(24);
  mInfoText.setFillColor(sf::Color::Red);

  // networking
  mBroadcastText.setFont(font);
  mBroadcastText.setPosition(5.f, 5.f);
  mBroadcastText.setCharacterSize(40);
  mBroadcastText.setFillColor(sf::Color::Red);

  // reuse
  mFailedConnectionText.setFont(context.fonts->get(Fonts::Main));
  mFailedConnectionText.setString("Attempting to connect...");
  mFailedConnectionText.setCharacterSize(35);
  mFailedConnectionText.setFillColor(sf::Color::Black);
  centerOrigin(mFailedConnectionText);
  mFailedConnectionText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f);

  // render "Connecting..."  text
  mWindow.clear(sf::Color::Black);
	mWindow.draw(mFailedConnectionText);
	mWindow.display();
	centerOrigin(mFailedConnectionText);

  // if host
  sf::IpAddress ip;
  if (isHost) {
    cout << "This instance is the host, spawining the game server" << endl;
    mGameServer.reset(new GameServer());
    ip = "127.0.0.1";
  } else {
    ip = getAddressFromFile();
  }

  cout << "attempting to connect to host" << endl;
  if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done) {
    cout << "Successfully connected to the server" << endl;
    cout << "ip: " << ip << endl;
    cout << "port: " << ServerPort << endl;
    mConnected = true;
  } else {
    // handle failed connection
    mFailedConnectionClock.restart();
  }

  mSocket.setBlocking(false);
}

void OnlineGameState::drawBoard (sf::RenderWindow& window) {
  window.clear(sf::Color(255,207,97));
  float midCell = 1.0 * mCellSize / 2;

  // Horizontal lines
  for (int x = 0; x < mBoardSize; x++) {
    sf::Vertex hline[] = {
      sf::Vertex(sf::Vector2f(midCell, midCell + x * mCellSize), sf::Color::Black),
      sf::Vertex(sf::Vector2f(mCellSize * mBoardSize - midCell, midCell + x * mCellSize), sf::Color::Black)
    };
    window.draw(hline, 2, sf::Lines);
  };

  // Vertical lines
  for (int y = 0; y < mBoardSize; y++) {
    sf::Vertex vline[] = {
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, midCell), sf::Color::Black),
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, mCellSize * mBoardSize - midCell), sf::Color::Black)
    };
    window.draw(vline, 2, sf::Lines);
  };

  // Draw start points
  float radius = midCell / 5;
  sf::CircleShape circle(radius);
  circle.setFillColor(sf::Color::Black);
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      int cells_between = 6;
      int xDistance = (mCellSize * cells_between) * x;
      int yDistance = (mCellSize * cells_between) * y;
      circle.setPosition( ((midCell + mCellSize * 3.f) + xDistance ) - radius,
                          ((midCell + mCellSize * 3.f) + yDistance) - radius);
      window.draw(circle);
    };
  };
}

void OnlineGameState::drawStones (sf::RenderWindow& window) {
  // Note that this doesn't work if window is resized!!
  for (int x = 0; x < mBoardSize; x++) {
    for (int y = 0; y < mBoardSize; y++) {
        if (mBoard[x][y] == Game::First) {
          mBlackStone.setPosition(x*mCellSize, y*mCellSize);
          window.draw(mBlackStone);
        }
        if (mBoard[x][y] == Game::Second) {
          mWhiteStone.setPosition(x*mCellSize, y*mCellSize);
          window.draw(mWhiteStone);
        }
    }
  }
}

void OnlineGameState::drawWinnerText(sf::RenderWindow& window) {
  window.draw(mInfoText);
}

bool OnlineGameState::isLegal(int x, int y){
  return mBoard[x][y] == 0 || false;
};

string OnlineGameState::getWinnerStr (Game::Turns& turn) {
  switch(turn) {
    case Game::First:
      return "Black";
      break;
    case Game::Second:
      return "White";
      break;
    default:
      return "Unrecognised option";
      break;
  }
};

// network
void OnlineGameState::handlePacket(sf::Int32 packetType, sf::Packet& packet) {
  switch (packetType) {
    case Server::PlayerDisconnect: {
      cout << "Server::PlayerDisconnect received";
    } break;

    case Server::BroadcastMessage: {
      std::string message;
      packet >> message;

      cout << "Received broadcast message: " << message << endl;

      mBroadcasts.push_back(message);

      // Display if immediately if only one message
      if (mBroadcasts.size() == 1) {
        cout << "Only one message displaying immediately: " << endl;
        cout << "message: " << mBroadcasts.front() << endl;
        mBroadcastText.setString(mBroadcasts.front());

        mBroadcastElapsedTime = sf::Time::Zero;
      }
    } break;

    case Server::GameStarted: {
      packet >> mGameStarted;
      cout << "Game started packet received" << endl;
    } break;

    case Server::PositionUpdate: {
      cout << "Received position update from the server" << endl;
      int x;
      int y;
      packet >> x;
      packet >> y;
      // update board
      mBoard[x][y] = mCurrentTurn;

      // only play stone click sound if it's other player's turn
      // otherwise it'll play sound twice
      if (!isMyTurn()) {
        playStoneClick();
      }

      //update(TimePerFrame);
      // determine whether it's the turn for this instance
    } break;

    case Server::PlayerData: {
      cout << "Player data packet received" << endl;
      //sf::Int32 playerTurn;
      int playerTurn;
      packet >> playerTurn;
      mPlayerTurn = (Game::Turns)playerTurn;
    } break;

    case Server::TurnUpdate: {
      cout << "Turn update received" << endl;
      sf::Int32 currentTurn;
      packet >> currentTurn;
      mCurrentTurn = (Game::Turns)currentTurn;
    } break;

    case Server::WinnerUpdate: {
      cout << "Winner update received" << endl;
      sf::Int32 winner;
      packet >> winner;
      mWinner = winner;
    } break;

    case Server::Quip: {
      cout << "Received quip" << endl;
      sf::Int32 effect;
      packet >> effect;
      cout << "Checking if quip is actually what we expect: "
           << (SoundEffect::NeedWork == (SoundEffect::ID)effect)
           << endl;
      mSounds.play((SoundEffect::ID)effect);
    } break;
  }
}

void OnlineGameState::updateBroadcastMessage(sf::Time elapsedTime){
  if (mBroadcasts.empty()) {
    return;
  }

  mBroadcastElapsedTime += elapsedTime;
  if (mBroadcastElapsedTime > sf::seconds(2.5f)) {
    // remove message if expired
    mBroadcasts.erase(mBroadcasts.begin());

    // Show next broadcast message if any
    if (!mBroadcasts.empty()) {
      mBroadcastText.setString(mBroadcasts.front());
      mBroadcastElapsedTime = sf::Time::Zero;
    }
  }

}

bool OnlineGameState::update (sf::Time dt) {
  if (mConnected) {
    // handle messages from server that may have arrived
    sf::Packet packet;
    if (mSocket.receive(packet) == sf::Socket::Done) {
      mTimeSinceLastPacket = sf::seconds(0.f);
      sf::Int32 packetType;
      packet >> packetType;
      handlePacket(packetType, packet);
    } else {
      // handle server timeout
      if (mTimeSinceLastPacket > mClientTimeout) {
        mConnected = false;
        mFailedConnectionText.setString("Lost connection to server");
        centerOrigin(mFailedConnectionText);
        mFailedConnectionClock.restart();
      }
    }

    // update broadcast message
    updateBroadcastMessage(dt);
    // events occuring in the game
    // send the updates to server

    if (mWinner != 0) {
      winnerStr = getWinnerStr(mCurrentTurn) + " has won!";
      mInfoText.setString(winnerStr);
    }

    mTimeSinceLastPacket += dt;
    
  } else if (mFailedConnectionClock.getElapsedTime() >= sf::seconds(30.f)){
    cout << "OnlineGameState::update timeout, going back to menu" << endl;
    requestStateClear();
    requestStackPush(States::Menu);
  }
  return true;
}

void OnlineGameState::drawBroadcast(sf::RenderWindow& window) {
  if (!mBroadcasts.empty()) {
    window.draw(mBroadcastText);
  }
}

void OnlineGameState::draw() {
  sf::RenderWindow& window = *getContext().window;

  if (mConnected) {
    drawBoard(window);
    drawStones(window);
    drawWinnerText(window);
    drawBroadcast(window);
  } else {
    mFailedConnectionText.setString("Could not connect to the remote server!");
    window.draw(mFailedConnectionText);
  }
}

void OnlineGameState::onActivate() {
  mActiveState = true;
}

void OnlineGameState::onDestroy() {
  if (!mIsHost && mConnected) {
    // inform server this client is being destroyed
    sf::Packet packet;
    packet << static_cast<sf::Int32>(Client::Quit);
    mSocket.send(packet);
  }
}

void OnlineGameState::sendPositionUpdates(int x, int y) {
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Client::PositionUpdate);
  packet << x;
  packet << y;
  cout << "Sending position updates" << endl;
  mSocket.send(packet);
}

void OnlineGameState::handleInput(const sf::Event& event) {
  if (event.type == sf::Event::MouseButtonPressed) {
      int ix = event.mouseButton.x / mCellSize;
      int iy = event.mouseButton.y / mCellSize;
      cout << "Button pressed" << endl;
      cout << "Mouse X: " << event.mouseButton.x << endl;
      cout << "Mouse Y: " << event.mouseButton.y << endl;
      cout << "ix: " << ix << endl;
      cout << "iy: " << iy << endl;
      if (event.mouseButton.button == sf::Mouse::Left && isLegal(ix, iy)) {
        mBoard[ix][iy] = mCurrentTurn;
        playStoneClick();
        sendPositionUpdates(ix, iy);
      } else {
        cout << "Cannot place your stone there, try again" << endl;
      }
    }
  else if (event.type == sf::Event::KeyPressed) {
    sendQuip(event);
  }
}

bool OnlineGameState::isMyTurn() {
  return mPlayerTurn == mCurrentTurn;
}

bool OnlineGameState::handleEvent(const sf::Event& event) {
  if (mGameStarted && isMyTurn()) {
    mInfoText.setString("Your turn");
    handleInput(event);
  } else {
    mInfoText.setString("Other player's turn");
  }
  return true;
}

void OnlineGameState::sendQuip(const sf::Event& event) {
  switch (event.key.code) {
    case sf::Keyboard::Q: {
      cout << "Sending Quip - NeedWork" << endl;
      mSounds.play(SoundEffect::NeedWork);
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Client::Quip);
      packet << static_cast<sf::Int32>(SoundEffect::NeedWork);
      mSocket.send(packet);
    } break;

    case sf::Keyboard::A: {
      cout << "Sending Quip - Namataro" << endl;
      mSounds.play(SoundEffect::Namataro);
      sf::Packet packet;
      packet << static_cast<sf::Int32>(Client::Quip);
      packet << static_cast<sf::Int32>(SoundEffect::Namataro);
      mSocket.send(packet);
    } break;
  }
}

void OnlineGameState::playStoneClick() {
  switch (mCurrentTurn) {
    case Game::First: {
      mSounds.play(SoundEffect::Stone1);
    } break;

    case Game::Second: {
      mSounds.play(SoundEffect::Stone2);
    } break;
  }
}
