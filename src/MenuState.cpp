#include "MenuState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

MenuState::MenuState(StateStack& stack, Context context)
: State(stack, context)
, mOptions()
, mOptionIndex(0)
{
  sf::Texture& texture = context.textures->get(Textures::TitleScreen);
  sf::Font& font = context.fonts->get(Fonts::Main);

  mBackgroundSprite.setTexture(texture);

  sf::Text playLocalOption;
  playLocalOption.setFont(font);
  playLocalOption.setString("Play");
  centerOrigin(playLocalOption);
  playLocalOption.setPosition(context.window->getView().getSize() / 2.f);
  mOptions.push_back(playLocalOption);

  sf::Text playOnlineClientOption;
  playOnlineClientOption.setFont(font);
  playOnlineClientOption.setString("Play Online as Client");
  centerOrigin(playOnlineClientOption);
  playOnlineClientOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 30.f));
  mOptions.push_back(playOnlineClientOption);

  sf::Text playOnlineHostOption;
  playOnlineHostOption.setFont(font);
  playOnlineHostOption.setString("Play Online as Host");
  centerOrigin(playOnlineHostOption);
  playOnlineHostOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 60.f));
  mOptions.push_back(playOnlineHostOption);

  sf::Text exitOption;
  exitOption.setFont(font);
  exitOption.setString("Exit");
  centerOrigin(exitOption);
  exitOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 90.f));
  mOptions.push_back(exitOption);

  updateOptionText();
};

void MenuState::draw() {
  sf::RenderWindow& window = *getContext().window;

  window.setView(window.getDefaultView());
  window.draw(mBackgroundSprite);

  for(const sf::Text& text : mOptions) {
    window.draw(text);
  }
}

bool MenuState::update(sf::Time) {
  return true;
}

bool MenuState::handleEvent(const sf::Event& event) {
  if (event.type != sf::Event::KeyPressed) {
    return false;
  }

  if (event.key.code == sf::Keyboard::Return) {
    if (mOptionIndex == PlayLocal) {
      requestStackPop();
      requestStackPush(States::Game);
    } else if (mOptionIndex == Exit) {
      requestStackPop();
    }
  }

  else if (event.key.code == sf::Keyboard::Up) {
    if (mOptionIndex > 0) {
      mOptionIndex--;
    } else {
      mOptionIndex = mOptions.size() - 1;
    }
    updateOptionText();
  }

  else if (event.key.code == sf::Keyboard::Down) {
    if (mOptionIndex < mOptions.size() - 1) {
      mOptionIndex++;
    } else {
      mOptionIndex = 0;
    }
    updateOptionText();
  }
  return true;
}

void MenuState::updateOptionText() {
  if (mOptions.empty()) {
    return;
  }

  for(sf::Text& text : mOptions) {
    text.setFillColor(sf::Color::White);
  }

  mOptions[mOptionIndex].setFillColor(sf::Color::Red);
}
