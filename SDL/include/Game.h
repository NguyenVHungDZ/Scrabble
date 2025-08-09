// -- include/Game.h --
#ifndef GAME_H
#define GAME_H

#include <SDL_rect.h> 

// Forward declarations
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
typedef struct TTF_Font TTF_Font;
class Board;
class Player;
class Tile;
class Dictionary;

enum GameState {
    PLAYING,
    GAME_OVER,
    MANUAL 
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
    void renderManual();

    bool isRunning;
    GameState currentState; 
    int highScore; 

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* mainFont; 
    TTF_Font* smallFont; 
    TTF_Font* uiFont;
    TTF_Font* gameOverFont; 
    SDL_Texture* manualIconTexture; 

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
    SDL_Rect manualButtonRect; 
};
#endif // GAME_H