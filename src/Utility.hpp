#ifndef __UTILITY_H_
#define __UTILITY_H_

#include <sstream>

namespace sf {
  class Sprite;
  class Text;
}

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);

#endif // __UTILITY_H_
