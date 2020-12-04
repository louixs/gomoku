#include "MenuState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <SFML/Network.hpp>

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
  playLocalOption.setString("Play Local");
  centerOrigin(playLocalOption);
  playLocalOption.setPosition(context.window->getView().getSize() / 2.f);
  mOptions.push_back(playLocalOption);

  sf::Text joinOnlineOption;
  joinOnlineOption.setFont(font);
  joinOnlineOption.setString("Join Online Game");
  centerOrigin(joinOnlineOption);
  joinOnlineOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 30.f));
  mOptions.push_back(joinOnlineOption);

  sf::Text hostOnlineOption;
  hostOnlineOption.setFont(font);
  hostOnlineOption.setString("Host Online Game");
  centerOrigin(hostOnlineOption);
  hostOnlineOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 60.f));
  mOptions.push_back(hostOnlineOption);

  sf::Text exitOption;
  exitOption.setFont(font);
  exitOption.setString("Exit");
  centerOrigin(exitOption);
  exitOption.setPosition(playLocalOption.getPosition() + sf::Vector2f(0.f, 90.f));
  mOptions.push_back(exitOption);

  updateOptionText();

  // Get Ip addresses

  std::cout << "Getting public ip" << std::endl;
  sf::Text publicIp;

  publicIp.setFont(font);
  publicIp.setString(
    "Public Ip: " +
    sf::IpAddress::getPublicAddress(sf::seconds(5.f)).toString()
  );
  publicIp.setPosition(5.f, 5.f);
  publicIp.setCharacterSize(20);
  publicIp.setFillColor(sf::Color::White);
  mAddresses.push_back(publicIp);

  std::cout << "Getting local ip" << std::endl;
  sf::Text localIp;

  localIp.setFont(font);
  localIp.setString(
    "Local Ip: " +
    sf::IpAddress::getLocalAddress().toString()
  );
  localIp.setPosition(5.f, 40.f);
  localIp.setCharacterSize(20);
  localIp.setFillColor(sf::Color::White);
  mAddresses.push_back(localIp);

};

void MenuState::draw() {
  sf::RenderWindow& window = *getContext().window;

  window.setView(window.getDefaultView());
  window.draw(mBackgroundSprite);

  for(const sf::Text& text : mOptions) {
    window.draw(text);
  }

  for (const sf::Text& text : mAddresses) {
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
    if (mOptionIndex == PlayLocalGame) {
      requestStackPop();
      requestStackPush(States::LocalGame);
    }
    else if (mOptionIndex == JoinOnlineGame) {
      requestStackPop();
      requestStackPush(States::SelectHost);
    }
    else if (mOptionIndex == HostOnlineGame) {
      requestStackPop();
      requestStackPush(States::HostOnlineGame);
    }
    else if (mOptionIndex == Exit) {
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
