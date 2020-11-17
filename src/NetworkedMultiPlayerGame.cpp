#include "NetworkedMultiPlayerGame.hpp"

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
  , mCurrentTurn(FIRST)
  , mWindow(sf::VideoMode(mCellSize * mBoardSize, mCellSize * mBoardSize),
                          "Gomoku",
                          sf::Style::Default,
                          mSettings)
  , mConnected(false)
  , mGameServer(nullptr)
  , mIsHost(isHost)
  , mGameStarted(false)
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
  mBroadcastText.setPosition(1024.f / 2, 100.f);

  // game server
  // if host
  sf::IpAddress ip;
  if (isHost) {
    mGameServer.reset(new GameServer());
    ip = "127.0.0.1";
  } else {
    ip = getAddressFromFile();
  }

  if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done) {
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

void NetworkedMultiplayerGame::update () {
  drawBoard();
  drawStones();
  drawWinnerText();
  mWindow.display();
}

void NetworkedMultiplayerGame::processEvents() {
  sf::Event event;
  while (mWindow.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      mWindow.close();
    };

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
        update();

        // check winner first -- after five turns to save some computation?
        if (hasWon(ix, iy)) {
          winnerStr = getWinnerStr(mCurrentTurn) + " has won!";
          mWinnerText.setString(winnerStr);
        };

        // if no winner, change turn
        changeTurn();
      } else {
        cout << "Cannot place your stone there, try again" << endl;
      }
    }
  }
  update();
}

void NetworkedMultiplayerGame::run() {
  while (mWindow.isOpen()) {
    processEvents();
  }
}
