#include "NetworkedMultiplayerGame.hpp"

#include <iostream>
#include <SFML/Network/IpAddress.hpp>

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

NetworkedMultiplayerGame::NetworkedMultiplayerGame(bool isHost)
  : mCellSize(40)
  , mTextures()
  , TimePerFrame(sf::seconds(1.f/5.f))
  , mCurrentTurn(FIRST)
  , mWindow(sf::VideoMode(mCellSize * mBoardSize, mCellSize * mBoardSize),
                          "Gomoku",
                          sf::Style::Default,
                          mSettings)
  , mConnected(false)
  , mGameServer(nullptr)
  , mIsHost(isHost)
  , mGameStarted(false)
  , mIsTurn(false)
  , mWinner(0)
{
  mSettings.antialiasingLevel = 8;
  initStones();

  // init fonts and texts
  mFonts.load(Fonts::Main, "assets/InputSerif-Light.ttf");
  sf::Font& mainFont = mFonts.get(Fonts::Main);

  // init winner text
  mInfoText.setFont(mainFont);
  mInfoText.setPosition(5.f, 5.f);
  mInfoText.setCharacterSize(24);
  mInfoText.setFillColor(sf::Color::Red);

  // networking
  mBroadcastText.setFont(mainFont);
  mBroadcastText.setPosition(5.f, 5.f);
  mBroadcastText.setCharacterSize(40);
  mBroadcastText.setFillColor(sf::Color::Red);

  // game server
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
  }

  mSocket.setBlocking(false);
}

void NetworkedMultiplayerGame::loadTextures() {
  mTextures.load(Textures::BlackStone, "assets/black_stone.bmp");
  mTextures.load(Textures::WhiteStone, "assets/white_stone.bmp");
}

void NetworkedMultiplayerGame::initStones() {
  // load stone textures
  loadTextures();
  
  auto& blackStoneTexture = mTextures.get(Textures::BlackStone);
  blackStoneTexture.setSmooth(true);

  auto& whiteStoneTexture = mTextures.get(Textures::WhiteStone);
  whiteStoneTexture.setSmooth(true);

  // set stone sprites
  mBlackStone.setTexture(blackStoneTexture);
  mWhiteStone.setTexture(whiteStoneTexture);

  // set stone scales
  mBlackStone.setScale(1.0*mCellSize / mBlackStone.getLocalBounds().width,
                       1.0*mCellSize / mBlackStone.getLocalBounds().height);
  mWhiteStone.setScale(1.0*mCellSize / mWhiteStone.getLocalBounds().width,
                       1.0*mCellSize / mWhiteStone.getLocalBounds().height);
}

void NetworkedMultiplayerGame::drawBoard () {
  mWindow.clear(sf::Color(255,207,97));
  float midCell = 1.0 * mCellSize / 2;

  // Horizontal lines
  for (int x = 0; x < mBoardSize; x++) {
    sf::Vertex hline[] = {
      sf::Vertex(sf::Vector2f(midCell, midCell + x * mCellSize), sf::Color::Black),
      sf::Vertex(sf::Vector2f(mCellSize * mBoardSize - midCell, midCell + x * mCellSize), sf::Color::Black)
    };
    mWindow.draw(hline, 2, sf::Lines);
  };

  // Vertical lines
  for (int y = 0; y < mBoardSize; y++) {
    sf::Vertex vline[] = {
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, midCell), sf::Color::Black),
      sf::Vertex(sf::Vector2f(midCell + y * mCellSize, mCellSize * mBoardSize - midCell), sf::Color::Black)
    };
    mWindow.draw(vline, 2, sf::Lines);
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
      mWindow.draw(circle);
    };
  };
}

void NetworkedMultiplayerGame::drawStones () {
  // Note that this doesn't work if window is resized!!
  for (int x = 0; x < mBoardSize; x++) {
    for (int y = 0; y < mBoardSize; y++) {
        if (mBoard[x][y] == BLACK) {
          mBlackStone.setPosition(x*mCellSize, y*mCellSize);
          mWindow.draw(mBlackStone);
        }
        if (mBoard[x][y] == WHITE) {
          mWhiteStone.setPosition(x*mCellSize, y*mCellSize);
          mWindow.draw(mWhiteStone);
        }
    }
  }
}

void NetworkedMultiplayerGame::drawWinnerText() {
  mWindow.draw(mInfoText);
}

bool NetworkedMultiplayerGame::isLegal(int x, int y){
  return mBoard[x][y] == 0 || false;
};

string NetworkedMultiplayerGame::getWinnerStr (int stone) {
  switch(stone) {
    case 1:
      return "Black";
      break;
    case 2:
      return "White";
      break;
    default:
      return "Unrecognised option";
      break;
  }
};

// network
void NetworkedMultiplayerGame::handlePacket(sf::Int32 packetType, sf::Packet& packet) {
  switch (packetType) {
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
      // determine whether it's the turn for this instance
    } break;

    case Server::PlayerData: {
      cout << "Player data packet received" << endl;
      //sf::Int32 playerTurn;
      int playerTurn;
      packet >> playerTurn;
      mPlayerTurn = (turns)playerTurn;
    } break;

    case Server::TurnUpdate: {
      cout << "Turn update received" << endl;
      sf::Int32 currentTurn;
      packet >> currentTurn;
      mCurrentTurn = (turns)currentTurn;
    } break;

    case Server::WinnerUpdate: {
      cout << "Winner update received" << endl;
      sf::Int32 winner;
      packet >> winner;
      mWinner = winner;
    } break;
  }
}

void NetworkedMultiplayerGame::updateBroadcastMessage(sf::Time elapsedTime){
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

void NetworkedMultiplayerGame::update (sf::Time dt) {
  if (mConnected) {
    // handle messages from server that may have arrived
    sf::Packet packet;
    if (mSocket.receive(packet) == sf::Socket::Done) {
      // mTimeSinceLastPacket = sf::seconds(0.f);
      sf::Int32 packetType;
      packet >> packetType;
      handlePacket(packetType, packet);
    } else {
      // handle server timeout
    }

    // update broadcast message
    updateBroadcastMessage(dt);
    // events occuring in the game
    // send the updates to server

    if (mWinner != 0) {
      winnerStr = getWinnerStr(mCurrentTurn) + " has won!";
      mInfoText.setString(winnerStr);
    }
  } else {
  }
}

void NetworkedMultiplayerGame::drawBroadcast() {
  if (!mBroadcasts.empty()) {
    mWindow.draw(mBroadcastText);
  }
}

void NetworkedMultiplayerGame::render() {
  drawBoard();
  drawStones();
  drawWinnerText();

  drawBroadcast();
  mWindow.display();
}

void NetworkedMultiplayerGame::sendPositionUpdates(int x, int y) {
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Client::PositionUpdate);
  packet << x;
  packet << y;
  cout << "Sending position updates" << endl;
  mSocket.send(packet);
}

void NetworkedMultiplayerGame::handleInput(sf::Event event) {
  if (event.type == sf::Event::MouseButtonPressed) {
      int ix = event.mouseButton.x / mCellSize;
      int iy = event.mouseButton.y / mCellSize;
      cout << "Button pressed" << endl;
      cout << "Mouse X: " << event.mouseButton.x << endl;
      cout << "Mouse Y: " << event.mouseButton.y << endl;
      cout << "ix: " << ix << endl;
      cout << "iy: " << iy << endl;
      cout << "current turn: " << mCurrentTurn << endl;
      if (event.mouseButton.button == sf::Mouse::Left && isLegal(ix, iy)) {
        cout << "current turn right after clicking: " << mCurrentTurn << endl;
        mBoard[ix][iy] = mCurrentTurn;
        sendPositionUpdates(ix, iy);
        update(TimePerFrame);
      } else {
        cout << "Cannot place your stone there, try again" << endl;
      }
    }
}

void NetworkedMultiplayerGame::processEvents() {
  sf::Event event;
  while (mWindow.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      mWindow.close();
    };
    bool isTurn = mPlayerTurn == mCurrentTurn;
    if (mGameStarted && isTurn) {
      mInfoText.setString("Your turn");
      handleInput(event);
    } else {
      mInfoText.setString("Other player's turn");
    }
  }
}

void NetworkedMultiplayerGame::run() {
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (mWindow.isOpen()) {
    sf::Time dt = clock.restart();
    timeSinceLastUpdate += dt;
    while (timeSinceLastUpdate > TimePerFrame) {
      timeSinceLastUpdate -= TimePerFrame;

      processEvents();
      update(TimePerFrame);
    }
    render();
  }
}
