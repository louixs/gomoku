#ifndef __RESOURCEIDENTIFIERS_H_
#define __RESOURCEIDENTIFIERS_H_

// Forward declaration of SFM: classes
namespace sf {
  class Texture;
  class Font;
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

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;

#endif // __RESOURCEIDENTIFIERS_H_
