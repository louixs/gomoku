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

  private:
    void updateOptionText();
    void addMenuOption(sf::Text& t,
                       sf::Font& font,
                       const std::string& s,
                       const sf::Vector2f& position);

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
    std::vector<sf::Text> mAddresses;
};


#endif // __MENUSTATE_H_
