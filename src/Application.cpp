#include "Application.hpp"
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "LocalGameState.hpp"
#include "OnlineGameState.hpp"

#include <iostream>
using namespace std;

const sf::Time Application::TimePerFrame = sf::seconds(1.f/30.f);

Application::Application()
: mCellSize(40)
, mWindow(sf::VideoMode(mCellSize * mBoardSize, mCellSize * mBoardSize),
          "Gomoku",
          sf::Style::Default,
          mSettings)
, mTextures()
, mFonts()
, mMusic()
, mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic))
{
  mSettings.antialiasingLevel = 8;
  mFonts.load(Fonts::Main, "assets/InputSerif-Light.ttf");
  mTextures.load(Textures::TitleScreen, "assets/title.png");
  mTextures.load(Textures::BlackStone, "assets/black_stone.bmp");
  mTextures.load(Textures::WhiteStone, "assets/white_stone.bmp");
  registerStates();

  mStateStack.pushState(States::Title);

  mMusic.setVolume(25.f);
}

void Application::run() {
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (mWindow.isOpen()) {
    sf::Time dt = clock.restart();
    timeSinceLastUpdate += dt;
    while (timeSinceLastUpdate > TimePerFrame) {
      timeSinceLastUpdate -= TimePerFrame;
      processInput();
      update(TimePerFrame);

      if (mStateStack.isEmpty()) {
        mWindow.close();
      }
    }
    render();
  }
}

void Application::processInput() {
  sf::Event event;

  while (mWindow.pollEvent(event)) {

    mStateStack.handleEvent(event);
    if (event.type == sf::Event::Closed) {
      mWindow.close();
    }
  }

}

void Application::update(sf::Time dt) {
  mStateStack.update(dt);
}

void Application::render() {
  mWindow.clear();
  mStateStack.draw();
  mWindow.setView(mWindow.getDefaultView());
  mWindow.display();
}

void Application::registerStates() {
  mStateStack.registerState<TitleState>(States::Title);
  mStateStack.registerState<MenuState>(States::Menu);
  mStateStack.registerState<LocalGameState>(States::LocalGame);
  mStateStack.registerState<OnlineGameState>(States::JoinOnlineGame, false);
  mStateStack.registerState<OnlineGameState>(States::HostOnlineGame, true);
}
