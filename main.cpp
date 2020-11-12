#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

const int cell_size = 40;
int board_size = 19;
int board[19][19] = { 0 };
#define BLACK (1)
#define WHITE (2)

void draw_board (sf::RenderWindow& window) {
  window.clear(sf::Color(255,207,97));
  float mid_cell = 1.0 * cell_size / 2;

  // Horizontal lines
  for (int x = 0; x < board_size; x++) {
    sf::Vertex hline[] = {
      sf::Vertex(sf::Vector2f(mid_cell, mid_cell + x * cell_size), sf::Color::Black),
      sf::Vertex(sf::Vector2f(cell_size * board_size - mid_cell, mid_cell + x * cell_size), sf::Color::Black)
    };
    window.draw(hline, 2, sf::Lines);
  };

  // Vertical lines
  for (int y = 0; y < board_size; y++) {
    sf::Vertex vline[] = {
      sf::Vertex(sf::Vector2f(mid_cell + y * cell_size, mid_cell), sf::Color::Black),
      sf::Vertex(sf::Vector2f(mid_cell + y * cell_size, cell_size * board_size - mid_cell), sf::Color::Black)
    };
    window.draw(vline, 2, sf::Lines);
  };

  // Draw start points
  float radius = mid_cell / 5;
  sf::CircleShape circle(radius);
  circle.setFillColor(sf::Color::Black);
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      int cells_between = 6;
      int x_distance = (cell_size * cells_between) * x;
      int y_distance = (cell_size * cells_between) * y;
      circle.setPosition( ((mid_cell + cell_size * 3.f) + x_distance ) - radius,
                          ((mid_cell + cell_size * 3.f) + y_distance) - radius);
      window.draw(circle);
    };
  };
}

void draw_stones (sf::RenderWindow& window, sf::Sprite& black_stone, sf::Sprite& white_stone) {
  for (int y = 0; y < board_size; y++) {
    for (int x = 0; x < board_size; x++) {
        if (board[y][x] == BLACK) {
          black_stone.setPosition(x*cell_size, y*cell_size);
          window.draw(black_stone);
        }
        if (board[y][x] == WHITE) {
          white_stone.setPosition(x*cell_size, y*cell_size);
          window.draw(white_stone);
        }
    }
  }
}

void update (sf::RenderWindow& window, sf::Sprite& black_stone, sf::Sprite& white_stone) {

  draw_board(window);
  draw_stones(window, black_stone, white_stone);
  window.display();
}

int main() {
  sf::ContextSettings s;
  s.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(cell_size * board_size, cell_size * board_size),
                          "Gomoku",
                          sf::Style::Default,
                          s);

  // load stones and prep
  sf::Texture black_stone_t;
  black_stone_t.loadFromFile("black_stone.bmp");
  black_stone_t.setSmooth(true);
	sf::Texture white_stone_t;
  white_stone_t.loadFromFile("white_stone.bmp");
  white_stone_t.setSmooth(true);

  sf::Sprite black_stone(black_stone_t);
  sf::Sprite white_stone(white_stone_t);
  black_stone.setScale(1.0*cell_size / black_stone.getLocalBounds().width,
                       1.0*cell_size / black_stone.getLocalBounds().height);
  white_stone.setScale(1.0*cell_size / white_stone.getLocalBounds().width,
                       1.0*cell_size / white_stone.getLocalBounds().height);

  // Game loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      };

      if (event.type == sf::Event::MouseButtonPressed) {
        int ix = event.mouseButton.x / cell_size;
        int iy = event.mouseButton.y / cell_size;
        cout << "Button pressed" << endl;
        cout << "Mouse X: " << event.mouseButton.x << endl;
        cout << "Mouse Y: " << event.mouseButton.y << endl;
        cout << "ix: " << ix << endl;
        cout << "iy: " << iy << endl;
        if (event.mouseButton.button == sf::Mouse::Left) {
          board[iy][ix] = BLACK;
          update(window, black_stone, white_stone);
        }
      }
    }
    update(window, black_stone, white_stone);
  }

  return 0;
}
