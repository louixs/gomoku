#ifndef __UTILITY_H_
#define __UTILITY_H_

#include <sstream>

namespace sf {
  class Sprite;
  class Text;
}

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);
std::string run(const std::string& command);

#endif // __UTILITY_H_
