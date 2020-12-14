#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include <vector>

class Application {
  public:
    Application();
    void run();

  private:
    void processInput();
    void update(sf::Time dt);
    void render();
    void registerStates();

  private:
    int mCellSize;
    static const sf::Time TimePerFrame;
    sf::ContextSettings mSettings;
    sf::RenderWindow mWindow;
    TextureHolder mTextures;
    FontHolder mFonts;
    MusicPlayer mMusic;
    SoundPlayer mSounds;
    StateStack mStateStack;
};



#endif // __APPLICATION_H_
