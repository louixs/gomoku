#include "OnlineGameState.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"
#include "Utility.hpp"
#include "GameUtility.hpp"
#include "Globals.hpp"

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
  , mBoard(g_boardSize, std::vector<int>(g_boardSize, 0))
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
  mInfoText.setCharacterSize(24);
  mInfoText.setFillColor(sf::Color::Red);
  mInfoText.setPosition(5.f, 5.f);

  // networking
  mBroadcastText.setFont(font);
  mBroadcastText.setPosition(5.f, 5.f);
  mBroadcastText.setCharacterSize(40);
  mBroadcastText.setFillColor(sf::Color::Red);

  // reuse
  mFailedConnectionText.setFont(context.fonts->get(Fonts::Main));
  mFailedConnectionText.setString("Attempting to connect...");
  mFailedConnectionText.setCharacterSize(35);
  mFailedConnectionText.setFillColor(sf::Color::White);
  centerOrigin(mFailedConnectionText);
  mFailedConnectionText.setPosition(context.window->getSize().x / 2.f,
                                    context.window->getSize().y / 2.f);

  // render "Connecting..."  text
  mWindow.clear(sf::Color::Black);
  mWindow.draw(mFailedConnectionText);
  mWindow.display();


  // if host
  sf::IpAddress ip;
  if (isHost) {
    cout << "This instance is the host, spawning the game server" << endl;
    mGameServer.reset(new GameServer());
    ip = "127.0.0.1";
  } else {
    cout << "Global var hostIp: " << g_hostIp << endl;
    ip = g_hostIp;
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
  window.clear(sf::Color(255,207,97)); // brow-ish board color
  float midCell = 1.0 * mCellSize / 2;

  // Horizontal lines
  for (int x = 0; x < g_boardSize; x++) {
    sf::Vertex hline[] = {
      sf::Vertex(sf::Vector2f(midCell, midCell + x * mCellSize), sf::Color::Black),
      sf::Vertex(sf::Vector2f(mCellSize * g_boardSize - midCell, midCell + x * mCellSize), sf::Color::Black)
    };
    window.draw(hline, 2, sf::Lines);
  };

  // Vertical lines
  for (int y = 0; y < g_boardSize; y++) {
    sf::Vertex vline[] = {
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, midCell), sf::Color::Black),
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, mCellSize * g_boardSize - midCell), sf::Color::Black)
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
  for (int x = 0; x < g_boardSize; x++) {
    for (int y = 0; y < g_boardSize; y++) {
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

    mTimeSinceLastPacket += dt;
    
  } else if (mFailedConnectionClock.getElapsedTime() >= sf::seconds(60.f)){
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
    drawInfoText(window);
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

void OnlineGameState::handleMouseInput(const sf::Event& event) {
  if (event.type == sf::Event::MouseButtonPressed) {
      int ix = event.mouseButton.x / mCellSize;
      int iy = event.mouseButton.y / mCellSize;
      cout << "Button pressed" << endl;
      cout << "Mouse X: " << event.mouseButton.x << endl;
      cout << "Mouse Y: " << event.mouseButton.y << endl;
      cout << "ix: " << ix << endl;
      cout << "iy: " << iy << endl;
      if (event.mouseButton.button == sf::Mouse::Left && GameUtility::isLegal(mBoard, ix, iy)) {
        // since current turn is an int, it also represens the stone of the player
        // to avoid duplication
        mBoard[ix][iy] = mCurrentTurn;
        playStoneClick();
        sendPositionUpdates(ix, iy);
      } else {
        cout << "Cannot place your stone there, try again" << endl;
      }
    }
}

void OnlineGameState::handleKeyInput(const sf::Event& event) {
  if (event.type == sf::Event::KeyPressed) {
    handleQuips(event);

    if (mWinner) {
      goToMenu(event);
    }
  }
}

bool OnlineGameState::isMyTurn() {
  return mPlayerTurn == mCurrentTurn;
}

void OnlineGameState::drawInfoText(sf::RenderWindow& window) {

  if (mGameStarted) {

    if (isMyTurn()) {
      mInfoText.setString("Your turn");
    } else {
      mInfoText.setString("Other player's turn");
    }

    if (mWinner) {
      mInfoText.setString(GameUtility::getGameEndStr(mCurrentTurn, mPlayerTurn));
    }

  } else {
    mInfoText.setString("Waiting for game to start");
  }

  window.draw(mInfoText);
}

bool OnlineGameState::handleEvent(const sf::Event& event) {
  if (mGameStarted) {

    if (isMyTurn() & !mWinner) {
      handleMouseInput(event);
    }

    handleKeyInput(event);
  } 
  return true;
}

void OnlineGameState::goToMenu(const sf::Event& event) {
  switch (event.key.code) {
    case sf::Keyboard::Escape: {
      requestStackPop();
      requestStackPush(States::Menu);
    } break;
  }
}

void OnlineGameState::sendQuip(SoundEffect::ID id) {
  cout << "Sending Quip: " << id << endl;
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Client::Quip);
  packet << static_cast<sf::Int32>(id);
  mSocket.send(packet);
}

void OnlineGameState::handleQuips(const sf::Event& event) {
  switch (event.key.code) {
    case sf::Keyboard::G: {
      sendQuip(SoundEffect::Greetings);
    } break;
      
    case sf::Keyboard::L: {
      sendQuip(SoundEffect::HavingLaungh);
    } break;

    case sf::Keyboard::M: {
      sendQuip(SoundEffect::MissionCompleted);
    } break;

    case sf::Keyboard::S: {
      sendQuip(SoundEffect::SomePeople);
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
