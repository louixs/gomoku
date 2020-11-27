#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
//#include "Player.hpp"
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
    static const int mBoardSize = 19;
    int mBoard[mBoardSize][mBoardSize] = { 0 };
    static const sf::Time TimePerFrame;
    sf::ContextSettings mSettings;
    sf::RenderWindow mWindow;
    TextureHolder mTextures;
    FontHolder mFonts;
    //Player mPlayer;
    MusicPlayer mMusic;
    SoundPlayer mSounds;
    StateStack mStateStack;
};



#endif // __APPLICATION_H_
