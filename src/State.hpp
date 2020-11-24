#ifndef __STATE_H_
#define __STATE_H_

#include "StateIdentifiers.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace sf
{
  class RenderWindow;
}

class StateStack;
class MusicPlayer;
//class Player;

class State {
  public:
    typedef std::unique_ptr<State> Ptr;

    struct Context {
      Context(sf::RenderWindow& window,
              TextureHolder& textures,
              FontHolder& fonts,
              MusicPlayer& music
              //, Player& player
      );
      sf::RenderWindow* window;
      TextureHolder* textures;
      FontHolder* fonts;
      MusicPlayer* music;
      //Player* player;
    } ;

  public:
    State(StateStack& stack, Context context);
    virtual ~State();
    virtual void draw() = 0;
    virtual bool update(sf::Time dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;

  protected:
    void requestStackPush(States::ID stateID);
    void requestStackPop();
    void requestStateClear();

    Context getContext() const;

  private:
    StateStack* mStack;
    Context mContext;
};

#endif // __STATE_H_
