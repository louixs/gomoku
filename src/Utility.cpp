#include "Utility.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
#include <fstream>
#include <iostream>

void centerOrigin(sf::Sprite& sprite) {
  sf::FloatRect bounds = sprite.getLocalBounds();
  sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

void centerOrigin(sf::Text& text) {
  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height/ 2.f));
}

// run shell command and return result as a string
std::string run(const std::string& command) {
  system((command + " > temp.txt").c_str());

  std::ifstream inputFile("temp.txt");
  std::string ret;

  // put the contents of the file
  inputFile >> ret;

  // remove the file
  if (std::remove("temp.txt") != 0) {
    perror("Error deleting temporary file");
  }

  return ret;
}
