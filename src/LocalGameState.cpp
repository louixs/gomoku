#include "LocalGameState.hpp"
#include "ResourceHolder.hpp"
#include "MusicPlayer.hpp"
#include "GameUtility.hpp"

#include <iostream>

using namespace std;

LocalGameState::LocalGameState(StateStack& stack, Context context)
  : State(stack, context)
  , mBoard(mBoardSize, std::vector<int>(mBoardSize, 0))
  , mCellSize(40)
  , mCurrentTurn(FIRST)
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

  mInfoText.setFont(font);
  mInfoText.setPosition(5.f, 5.f);
  mInfoText.setCharacterSize(24);
  mInfoText.setFillColor(sf::Color::Red);
}

void LocalGameState::drawBoard (sf::RenderWindow& window) {
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

void LocalGameState::drawStones (sf::RenderWindow& window) {
  // Note that this doesn't work if window is resized!!
  for (int x = 0; x < mBoardSize; x++) {
    for (int y = 0; y < mBoardSize; y++) {
        if (mBoard[x][y] == BLACK) {
          mBlackStone.setPosition(x*mCellSize, y*mCellSize);
          window.draw(mBlackStone);
        }
        if (mBoard[x][y] == WHITE) {
          mWhiteStone.setPosition(x*mCellSize, y*mCellSize);
          window.draw(mWhiteStone);
        }
    }
  }
}

void LocalGameState::drawWinnerText(sf::RenderWindow& window) {
  window.draw(mInfoText);
}

void LocalGameState::changeTurn () {
  if (mCurrentTurn == FIRST) {
    mCurrentTurn = SECOND;
  } else {
    mCurrentTurn = FIRST;
  }
}

string LocalGameState::getWinnerStr (int stone) {
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

bool LocalGameState::update (sf::Time) {
  return true;
}

void LocalGameState::draw () {
  sf::RenderWindow& window = *getContext().window;
  drawBoard(window);
  drawStones(window);
  drawWinnerText(window);
}

bool LocalGameState::handleEvent(const sf::Event& event) {

  if (event.type == sf::Event::MouseButtonPressed) {
    int ix = event.mouseButton.x / mCellSize;
    int iy = event.mouseButton.y / mCellSize;
    cout << "Button pressed" << endl;
    cout << "Mouse X: " << event.mouseButton.x << endl;
    cout << "Mouse Y: " << event.mouseButton.y << endl;
    cout << "ix: " << ix << endl;
    cout << "iy: " << iy << endl;
    cout << "current turn: " << mCurrentTurn << endl;
    if (event.mouseButton.button == sf::Mouse::Left && GameUtility::isLegal(mBoard, ix, iy)) {
      mBoard[ix][iy] = mCurrentTurn;
      draw();

      // check winner first -- after five turns to save some computation?
      if (GameUtility::hasWon(mBoard, mBoardSize, mWinStoneCount, mCurrentTurn, ix, iy)) {
        winnerStr = getWinnerStr(mCurrentTurn) + " has won!";
        mInfoText.setString(winnerStr);
      };

      // if no winner, change turn
      changeTurn();
    } else {
      cout << "Cannot place your stone there, try again" << endl;
    }
  }
  return true;
}
