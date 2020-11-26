#ifndef __RESOURCEIDENTIFIERS_H_
#define __RESOURCEIDENTIFIERS_H_

// Forward declaration of SFM: classes
namespace sf {
  class Texture;
  class Font;
  class SoundBuffer;
}

namespace Textures {
  enum ID {
    BlackStone,
    WhiteStone,
    TitleScreen
  };
}

namespace Fonts {
  enum ID {
    Main
  };
}

namespace Music {
  enum ID
    {
      MenuTheme
    };
}

namespace SoundEffect {
  enum ID {
    Quip
  };
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect::ID> SoundBufferHolder;

#endif // __RESOURCEIDENTIFIERS_H_
