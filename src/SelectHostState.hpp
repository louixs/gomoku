#ifndef __ENTERHOSTSTATE_H_
#define __ENTERHOSTSTATE_H_

#include "State.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class SelectHostState : public State {
  public:
    SelectHostState(StateStack& stack, Context context);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);

  private:
    sf::Sprite mBackgroundSprite;
    std::string mHostIpStr;
    sf::Text mHostIp;
    sf::Text mPrompt;
};


#endif // __ENTERHOSTSTATE_H_
