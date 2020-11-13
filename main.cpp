#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

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
  // Note that this doesn't work if window is resized!!
  for (int x = 0; x < board_size; x++) {
    for (int y = 0; y < board_size; y++) {
        if (board[x][y] == BLACK) {
          black_stone.setPosition(x*cell_size, y*cell_size);
          window.draw(black_stone);
        }
        if (board[x][y] == WHITE) {
          white_stone.setPosition(x*cell_size, y*cell_size);
          window.draw(white_stone);
        }
    }
  }
}

void draw_text (sf::RenderWindow& window, sf::Font& font, sf::Text t, string& s) {
  cout << "draw text: " << s << endl;
  t.setString(s);
  t.setPosition(5.f, 5.f);
  t.setCharacterSize(24);
  t.setFillColor(sf::Color::Red);
  window.draw(t);
}

void update (sf::RenderWindow& window, sf::Sprite& black_stone, sf::Sprite& white_stone, sf::Text text) {
  draw_board(window);
  draw_stones(window, black_stone, white_stone);
  window.draw(text);
  window.display();
}

enum turns { FIRST = BLACK, SECOND = WHITE };
inline void change_turn (enum turns& current_turn) {
  if (current_turn == FIRST) {
    current_turn = SECOND;
  } else {
    current_turn = FIRST;
  }
}


// TODO: refactor!
bool has_won(const int (&board)[19][19], int stone_color, int x, int y) {
  
  int total_count = 0;
  int current_color = 0;

  // vertical check
  // count up
  int y1 = y;
  do {
    if (total_count == 5) {
      return true;
    }
      
    current_color = board[x][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    y1--;
  } while (y1 > 0 && current_color == stone_color);

  // keep checking if the function hasn't exited
  // count down
  // reset params
  current_color = 0;
  y1 = y;
  // avoid double count
  total_count = total_count - 1;

  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x][y1];
    if (current_color == stone_color) {
      total_count++;
    }
    y1++;

  } while (y1 < board_size && current_color == stone_color);

  // Horizontal check
  current_color = 0;
  int x1 = x;

  // avoid double count
  total_count = total_count - 1;

  // check left
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1--;
  } while (x1 > 0 && current_color == stone_color);

  // check right
  // reset vars
  current_color = 0;
  x1 = x;

  // avoid double count
  total_count = total_count - 1;

  // check right
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1++;
  } while (x1 < board_size && current_color == stone_color);

  // DIAGONAL

  // reset vars
  current_color = 0;
  x1 = x;
  y1 = y;

  // avoid double count
  total_count = total_count - 1;

  // check left up
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1--;
    y1--;
  } while (x1 > 0 && y1 > 0 && current_color == stone_color);

  // reset vars
  current_color = 0;
  x1 = x;
  y1 = y;

  // avoid double count
  total_count = total_count - 1;

  // check left down
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1--;
    y1++;
  } while (x1 > 0 && y < board_size && current_color == stone_color);

  // reset vars
  current_color = 0;
  x1 = x;
  y1 = y;

  // avoid double count
  total_count = total_count - 1;

  // check right up
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1++;
    y1--;
  } while (x1 < board_size && y > 0 && current_color == stone_color);

  // reset vars
  current_color = 0;
  x1 = x;
  y1 = y;

  // avoid double count
  total_count = total_count - 1;

  // check right down
  do {
    if (total_count == 5) {
      return true;
    }

    current_color = board[x1][y1];
    // if the current color matches the color to check
    // increment the total count
    if (current_color == stone_color) {
      total_count++;
    }

    x1++;
    y1++;
  } while (x1 < board_size && y < board_size && current_color == stone_color);


  cout << "current total " << total_count << endl;

  return false;
};

inline bool is_legal(const int (&board)[19][19], int x, int y){
  return board[x][y] == 0 || false;
};

string get_winner_str (int stone) {
  switch(stone) {
    case 1:
      return "Black";
      break;
    case 2:
      return "White";
      break;
    default:
      return "Unrecognised option";
      break;
  }
};

int main() {
  sf::ContextSettings s;
  s.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(cell_size * board_size, cell_size * board_size),
                          "Gomoku",
                          sf::Style::Default,
                          s);
  // text
  sf::Font font;
  if (!font.loadFromFile("noto_sans.otf"))
  {
    cout << "font didn't load!" << endl;
    // error...
  }
  string victory_message;
  sf::Text text;
  text.setFont(font);
  text.setPosition(5.f, 5.f);
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::Red);
  // init turn
  turns current_turn = FIRST;

  // load stones and prep
  // TODO: refactor
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
        if (event.mouseButton.button == sf::Mouse::Left && is_legal(board, ix, iy)) {
          board[ix][iy] = current_turn;
          update(window, black_stone, white_stone, text);

          // check winner first -- after five turns to save some computation?
          if (has_won(board, current_turn, ix, iy)) {
            string winner = get_winner_str(current_turn);
            string victory_message = winner + "has won!";
            // won_text.setString(txt);
            // won_text.setPosition(100.f, 100.f);
            // draw_text(window, font, text, victory_message);
            text.setString(victory_message);
            cout << victory_message << endl;
          };

          // if no winner, change turn
          change_turn(current_turn);
        } else {
          cout << "Cannot place your stone there, try again" << endl;
        }
      }
    }
    update(window, black_stone, white_stone, text);
  }

  return 0;
}
