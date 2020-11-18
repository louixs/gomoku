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
  , TimePerFrame(sf::seconds(1.f/60.f)) // make it much slower than 60 fps as fast update is not necessary
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
{
  mSettings.antialiasingLevel = 8;
  // load stone textures
  if (!mBlackStoneTexture.loadFromFile("assets/black_stone.bmp")){
    cout << "failed to load texture" << endl;
  }
  mBlackStoneTexture.setSmooth(true);

  if (!mWhiteStoneTexture.loadFromFile("assets/white_stone.bmp")) {
    cout << "failed to load texture" << endl;
  }
  mWhiteStoneTexture.setSmooth(true);

  // set stone sprites
  mBlackStone.setTexture(mBlackStoneTexture);
  mWhiteStone.setTexture(mWhiteStoneTexture);

  // set stone scales
  mBlackStone.setScale(1.0*mCellSize / mBlackStone.getLocalBounds().width,
                       1.0*mCellSize / mBlackStone.getLocalBounds().height);
  mWhiteStone.setScale(1.0*mCellSize / mWhiteStone.getLocalBounds().width,
                       1.0*mCellSize / mWhiteStone.getLocalBounds().height);

  // init winner text
  if (!mMainFont.loadFromFile("assets/noto_sans.otf"))
  {
    cout << "font didn't load!" << endl;
    // error...
  }
  mWinnerText.setFont(mMainFont);
  mWinnerText.setPosition(5.f, 5.f);
  mWinnerText.setCharacterSize(24);
  mWinnerText.setFillColor(sf::Color::Red);

  // networking
  mBroadcastText.setFont(mMainFont);
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
  mWindow.draw(mWinnerText);
}

// TODO: refactor!
bool NetworkedMultiplayerGame::hasWon(int x, int y) {

  int total_count = 0;
  int current_color = 0;

  // VERTICAL check
  // count up
  int y1 = y;
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    y1--;
  } while (y1 > 0 && current_color == mCurrentTurn);

  // keep checking if the function hasn't exited
  // count down
  // reset params
  y1 = y;
  // avoid double count
  total_count = total_count - 1;

  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x][y1];
    if (current_color == mCurrentTurn) {
      total_count++;
    }
    y1++;

  } while (y1 < mBoardSize && current_color == mCurrentTurn);

  // Horizontal check
  // reset params
  int x1 = x;
  total_count = 0;

  // check left
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1--;
  } while (x1 > 0 && current_color == mCurrentTurn);

  // check right
  // reset vars
  x1 = x;

  // avoid double count
  total_count = total_count - 1;

  // check right
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1++;
  } while (x1 < mBoardSize && current_color == mCurrentTurn);

  // DIAGONAL

  // reset vars
  x1 = x;
  y1 = y;
  total_count = 0;

  // 1. LEFT UP RIGHT DOWN
  // check left up
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1--;
    y1--;
  } while (x1 > 0 && y1 > 0 && current_color == mCurrentTurn);

  // check right down
  // reset vars
  x1 = x;
  y1 = y;

  // avoid double count
  total_count = total_count - 1;

  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1++;
    y1++;
  } while (x1 < mBoardSize && y < mBoardSize && current_color == mCurrentTurn);


  // 2. LEFT TO RIGHT UP
  // reset vars
  x1 = x;
  y1 = y;
  total_count = 0;

  // check left down
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1--;
    y1++;
  } while (x1 > 0 && y < mBoardSize && current_color == mCurrentTurn);

  // right up
  // reset vars
  x1 = x;
  y1 = y;

  total_count = total_count - 1;

  do {
    if (total_count == 5) {
      return true;
    }

    current_color = mBoard[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == mCurrentTurn) {
      total_count++;
    }

    x1++;
    y1--;
  } while (x1 < mBoardSize && y > 0 && current_color == mCurrentTurn);

  return false;
};

bool NetworkedMultiplayerGame::isLegal(int x, int y){
  return mBoard[x][y] == 0 || false;
};

void NetworkedMultiplayerGame::changeTurn () {
  if (mCurrentTurn == FIRST) {
    mCurrentTurn = SECOND;
  } else {
    mCurrentTurn = FIRST;
  }
}

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

void NetworkedMultiplayerGame::setTurn () {
    mIsTurn = mPlayerTurn == mCurrentTurn;
}

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

    case Server::UpdateClientState: {
      cout << "received client update" << endl;
      sf::Int32 currentTurn;
      int x;
      int y;
      packet >> currentTurn;
      packet >> x;
      packet >> y;
      mCurrentTurn = (turns)currentTurn;
      // update board
      mBoard[x][y] = currentTurn;
      // determine whether it's the turn for this instance
      setTurn();
    } break;

    case Server::PlayerData: {
      cout << "Player data packet received" << endl;
      //sf::Int32 playerTurn;
      int playerTurn;
      packet >> playerTurn;
      mPlayerTurn = (turns)playerTurn;
      setTurn();
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

void NetworkedMultiplayerGame::sendPositionUpdates(turns& mCurrentTurn, int x, int y) {
  sf::Packet packet;
  packet << static_cast<sf::Int32>(Client::PositionUpdate);
  packet << static_cast<sf::Int32>(mCurrentTurn);
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
        mBoard[ix][iy] = mCurrentTurn;
        sendPositionUpdates(mCurrentTurn, ix, iy);
        update(TimePerFrame);

        // check winner first -- after five turns to save some computation?
        if (hasWon(ix, iy)) {
          winnerStr = getWinnerStr(mCurrentTurn) + " has won!";
          mWinnerText.setString(winnerStr);
        };
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

    if (mGameStarted && mIsTurn) {
      handleInput(event);
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
