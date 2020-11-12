#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

const int cell_size = 40;
int board_size = 19;
const int board[19][19] = { 0 }; // could not pass in variable here
sf::RenderWindow window(sf::VideoMode(cell_size * board_size, cell_size * board_size), "Gomoku");

void draw_board () {
  float mid_cell = 1.0 * cell_size / 2;

  for (int x = 0; x < board_size; x++) {
    sf::Vertex hline[] = {
      sf::Vertex(sf::Vector2f(mid_cell, mid_cell + x * cell_size), sf::Color::Black),
      sf::Vertex(sf::Vector2f(cell_size * board_size - mid_cell, mid_cell + x * cell_size), sf::Color::Black)
    };
    window.draw(hline, 2, sf::Lines);
  };

  for (int y = 0; y < board_size; y++) {
    sf::Vertex vline[] = {
      sf::Vertex(sf::Vector2f(mid_cell + y * cell_size, mid_cell), sf::Color::Black),
      sf::Vertex(sf::Vector2f(mid_cell + y * cell_size, cell_size * board_size - mid_cell), sf::Color::Black)
    };
    window.draw(vline, 2, sf::Lines);
  };

  sf::Vertex cell[] = {
    sf::Vertex(sf::Vector2f(mid_cell, mid_cell), sf::Color::Red),
    sf::Vertex(sf::Vector2f(cell_size + mid_cell, mid_cell), sf::Color::Red),
    sf::Vertex(sf::Vector2f(cell_size + mid_cell, cell_size + mid_cell), sf::Color::Red),
    sf::Vertex(sf::Vector2f(mid_cell, cell_size + mid_cell), sf::Color::Red),
};
  window.draw(cell, 4, sf::Quads);
}


void update () {
  window.clear(sf::Color(255,207,97));
  draw_board();
  window.display();
}

int main() {


  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      };
    }
    update();
  }

  return 0;
}
