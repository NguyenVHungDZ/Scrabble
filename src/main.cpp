// -- src/main.cpp --
#include "Game.h"
#include <iostream> 

using namespace std;

int main(int argc, char* args[]) {
    Game game;
    if (game.init()) {
        game.run();
    } else {
        cerr << "Game initialization failed. The program will now exit." << endl;
        cout << "Press Enter to continue...";
        cin.get();
    }
    return 0;
}