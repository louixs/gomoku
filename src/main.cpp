#include "LocalGame.cpp"
#include "NetworkedMultiPlayerGame.cpp"

namespace {
  bool isLocalGame() {
    char option = 'n';
    while (!(option == 'L' || option == 'O'))
    {
      std::cout << "[L]ocal or [O]nline game?\n";
      std::cin >> option;

      if (!(option == 'L' || option == 'O'))
      {
        std::cout << "Invalid option." << std::endl;
      }
    }

    return option == 'L';
  }


  bool isHost() {
    char option = 'n';

    while (!(option == 'C' || option == 'H'))
    {
      std::cout << "[C]lient or [H]ost?\n";
      std::cin >> option;

      if (!(option == 'C' || option == 'H'))
      {
        std::cout << "Invalid." << std::endl;
      }
    }

    return option == 'H';
  }
}

int main() {
  // select game mode
  if (isLocalGame()) {
    LocalGame game;
    game.run();
  } else {
    NetworkedMultiplayerGame game(isHost());
    game.run();
  }
}
