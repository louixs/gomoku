#include "SelectHostState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

#include "Globals.hpp"

#include <SFML/Network/IpAddress.hpp>
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

   // only accept numbers and dot
   // otherwise any key is passed including Enter
   if (event.text.unicode == 46 || // dot
       (48 <= event.text.unicode && event.text.unicode <= 57) // numbers
   ) {     
     mHostIpStr += (char)event.text.unicode;
     mHostIp.setString(mHostIpStr);
   }
 }

 // if key is enter and if ip address is valid
 // save ip address
 // and request next state stack
 if (event.type == sf::Event::KeyPressed) {

   switch (event.key.code) {
     case sf::Keyboard::Enter: {
       sf::IpAddress ip = mHostIpStr;

       if (ip == sf::IpAddress::None || // ip address is invalid OR
           (mHostIpStr.size() < 8)) // not enough input from the user
       {         
         mPrompt.setFillColor(sf::Color::Red);
         mPrompt.setString("Please re-enter host IP address");

         // reset string
         mHostIpStr = "";
         mHostIp.setString(mHostIpStr);
         break;
       }

       // otherwise
       // save ip address to the global var g_hostIp
       g_hostIp = ip;
       requestStackPop();
       requestStackPush(States::JoinOnlineGame);
     } break;

   // handle delete
     case sf::Keyboard::BackSpace: {
       if (mHostIpStr .size() > 0) {
         mHostIpStr.pop_back();
         mHostIp.setString(mHostIpStr);
       }
     } break;
   }
}

 return true;
}
