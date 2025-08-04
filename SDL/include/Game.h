// -- include/Game.h --
#ifndef GAME_H
#define GAME_H

#include <SDL_rect.h> 

// Forward declarations
struct SDL_Window;
struct SDL_Renderer;
typedef struct TTF_Font TTF_Font;
class Board;
class Player;
class Tile;
class Dictionary;

enum GameState {
    PLAYING,
    GAME_OVER
};

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
    void resetLetters(); 
    void renderUI();
    void loadHighScore();
    void saveHighScore();
    void startOver();
    void renderGameOver();

    bool isRunning;
    GameState currentState; 
    int highScore; 

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* mainFont;
    TTF_Font* smallFont;
    TTF_Font* uiFont;
    TTF_Font* gameOverFont; 

    Board* board;
    Player* player;
    Dictionary* dictionary;
    Tile* selectedTile;
    int mouseX, mouseY;

    // UI elements
    SDL_Rect submitButtonRect; 
    SDL_Rect recallButtonRect;
    SDL_Rect resetButtonRect;
    SDL_Rect startOverButtonRect; 
};
#endif // GAME_H