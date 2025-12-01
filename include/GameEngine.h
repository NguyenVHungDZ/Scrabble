#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Board.h"
#include "Player.h"
#include "Dictionary.h"
#include <string>

// Trạng thái game (Logic)
enum EngineState { PLAYING, GAME_OVER, MANUAL_VIEW };

struct TurnResult {
    bool success;
    std::string message;
    int scoreEarned;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void init();
    
    // Các hàm này bạn sẽ gọi trong Unit Test
    TurnResult submitWord();
    void resetLetters();
    void recallAllTiles();
    void startOver();
    
    // Getters
    int getHighScore() const { return highScore; }
    EngineState getState() const { return currentState; }
    void setState(EngineState s) { currentState = s; }

    // Public để dễ test (hoặc viết getter)
    Board* board;
    Player* player;
    Dictionary* dictionary;

private:
    void loadHighScore();
    void saveHighScore();
    
    int highScore;
    EngineState currentState;
};
#endif // GAME_ENGINE_H