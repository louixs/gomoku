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

  sf::Vector2f optionTopPosition = (context.window->getView().getSize() / 2.f);
  sf::Text playLocalOption;
  addMenuOption(playLocalOption, font, "Play Local", optionTopPosition);
  sf::Text joinOnlineOption;
  addMenuOption(joinOnlineOption, font, "Join Online Game", (optionTopPosition + sf::Vector2f(0.f, 30.f)));
  sf::Text hostOnlineOption;
  addMenuOption(hostOnlineOption, font, "Host Online Game", (optionTopPosition + sf::Vector2f(0.f, 60.f)));
  sf::Text exitOption;
  addMenuOption(exitOption, font, "Exit", (optionTopPosition + sf::Vector2f(0.f, 90.f)));

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
  publicIp.setFillColor(sf::Color::Black);
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
  localIp.setFillColor(sf::Color::Black);
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

void MenuState::addMenuOption(sf::Text& t,
                              sf::Font& font,
                              const std::string& s,
                              const sf::Vector2f& position) {
  std::cout << "setting font" << std::endl;
  t.setFont(font);
  t.setString(s);
  centerOrigin(t);
  t.setPosition(position);
  mOptions.push_back(t);
}

void MenuState::updateOptionText() {
  if (mOptions.empty()) {
    return;
  }

  for(sf::Text& text : mOptions) {
    text.setFillColor(sf::Color::Black);
  }

  mOptions[mOptionIndex].setFillColor(sf::Color::White);
}
