#ifndef __LOCALGAMESTATE_H_
#define __LOCALGAMESTATE_H_

#include "State.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class LocalGameState : public State {
  public:
    LocalGameState(StateStack& stack, Context context);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);


};


#endif // __LOCALGAMESTATE_H_
