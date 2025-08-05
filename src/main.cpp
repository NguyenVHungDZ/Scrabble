// -- src/main.cpp --
#include "Game.hpp"
#include <iostream> 

int main(int argc, char* args[]) {
    Game game;
    if (game.init()) {
        game.run();
    } else {
        std::cerr << "Game initialization failed. The program will now exit." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
    return 0;
}