// -- include/Game.h --
#ifndef GAME_H
#define GAME_H

#include <SDL_rect.h>
#include <vector>
#include <map>
#include <string>

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
    MAIN_MENU,
    NAME_ENTRY,
    PLAYING,
    GAME_OVER,
    MANUAL
};

enum GameMode {
    ONE_PLAYER,
    TWO_PLAYER
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

    void startGame(GameMode mode, std::string p1Name = "Player 1", std::string p2Name = "Player 2");
    void submitWord();
    void recallAllTiles();
    void resetLetters();
    void passTurn();
    void switchPlayer();
    void checkEndGameConditions();

    void renderMainMenu();
    void renderNameEntry();
    void renderUI();
    void renderGameOver();
    void renderManual();

    void loadHighScore();
    void saveHighScore();
    void startOver();

    // Game State & Mode
    bool isRunning;
    GameState currentState;
    GameMode gameMode;
    int highScore;
    bool isFirstTurn;
    int consecutiveScorelessTurns;

    // SDL & Fonts
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* mainFont;
    TTF_Font* smallFont;
    TTF_Font* uiFont;
    TTF_Font* gameOverFont;
    SDL_Texture* manualIconTexture;
    SDL_Texture* backButtonTexture; 

    // Game Objects
    Board* board;
    Dictionary* dictionary;
    Tile* selectedTile;
    int mouseX, mouseY;

    // Player Management
    Player* player1;
    Player* player2;
    Player* currentPlayer;
    int currentPlayerIndex;

    // Name Entry State
    std::string player1NameInput;
    std::string player2NameInput;
    int activeInputFieldIndex;
    bool player1NameClicked;
    bool player2NameClicked;

    // Shared Tile Bag
    std::vector<char> tileBag;
    std::map<char, int> tileValues;
    void initializeTileBag();

    // UI Elements
    SDL_Rect submitButtonRect;
    SDL_Rect recallButtonRect;
    SDL_Rect resetButtonRect;
    SDL_Rect passButtonRect;
    SDL_Rect startOverButtonRect;
    SDL_Rect manualButtonRect;
    SDL_Rect backButtonRect; 
    SDL_Rect inGameBackButtonRect; // New button for the game screen
    // Menu Buttons
    SDL_Rect onePlayerButtonRect;
    SDL_Rect twoPlayerButtonRect;
    // Name Entry Buttons
    SDL_Rect player1InputRect;
    SDL_Rect player2InputRect;
    SDL_Rect startGameButtonRect;
};
#endif
