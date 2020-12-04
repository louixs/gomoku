#include "SelectHostState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

SelectHostState::SelectHostState(StateStack& stack, Context context)
: State(stack, context)
{
  sf::Texture& texture = context.textures->get(Textures::TitleScreen);
  sf::Font& font = context.fonts->get(Fonts::Main);

  mBackgroundSprite.setTexture(texture);

  mPrompt.setFont(font);
  mPrompt.setString("Please enter host IP address");
  centerOrigin(mPrompt);
  mPrompt.setPosition(context.window->getView().getSize() / 2.f);

  mHostIp.setFont(font);
  centerOrigin(mHostIp);
  mHostIp.setPosition(mPrompt.getPosition() + sf::Vector2f(0.f, 30.f));

}

void SelectHostState::draw() {
  sf::RenderWindow& window = *getContext().window;

  window.setView(window.getDefaultView());
  window.draw(mBackgroundSprite);

  window.draw(mPrompt);
  window.draw(mHostIp);
}

bool SelectHostState::update(sf::Time) {
  return true;
}

bool SelectHostState::handleEvent(const sf::Event& event) {
 // if (event.type != sf::Event::KeyPressed) {
 //    return false;
 //  }

 if (event.type == sf::Event::TextEntered) {
   std::cout << "Text entered" << std::endl;
   if (event.text.unicode < 128) {
     mHostIpStr += static_cast<char>(event.text.unicode);
     mHostIp.setString(mHostIpStr);
   }
 }

 return true;
}
