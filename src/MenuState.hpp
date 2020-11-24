#ifndef __MENUSTATE_H_
#define __MENUSTATE_H_

#include "State.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class MenuState : public State {
  public:
    MenuState(StateStack& stack, Context context);
    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);

    void updateOptionText();

  private:
    enum OptionNames {
      PlayLocalGame,
      JoinOnlineGame,
      HostOnlineGame,
      Exit
    };

  private:
    sf::Sprite mBackgroundSprite;
    std::vector<sf::Text> mOptions;
    std::size_t mOptionIndex;
};


#endif // __MENUSTATE_H_
