// -- include/Game.h --
#ifndef GAME_H
#define GAME_H

#include <SDL_rect.h> // Include the definition for SDL_Rect

// Forward declarations
struct SDL_Window;
struct SDL_Renderer;
typedef struct TTF_Font TTF_Font;
class Board;
class Player;
class Tile;
class Dictionary;

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
private:
    void handleEvents();
    void update();
    void render();
    void cleanup();
    void submitWord();
    void recallAllTiles();
    void resetLetters(); // NEW: Action for the reset button
    void renderUI();
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* mainFont;
    TTF_Font* smallFont;
    TTF_Font* uiFont;
    Board* board;
    Player* player;
    Dictionary* dictionary;
    Tile* selectedTile;
    int mouseX, mouseY;
    SDL_Rect submitButtonRect; 
    SDL_Rect recallButtonRect;
    SDL_Rect resetButtonRect; // NEW: Rect for the reset button
};
#endif // GAME_H