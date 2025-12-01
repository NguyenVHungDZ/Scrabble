#include "GameEngine.h"
#include <iostream>
#include <fstream>

// Constructor: Khởi tạo các thành phần logic
GameEngine::GameEngine() 
    : highScore(0), currentState(EngineState::PLAYING), 
      board(nullptr), player(nullptr), dictionary(nullptr) 
{
}

GameEngine::~GameEngine() {
    saveHighScore();
    delete board;
    delete player;
    delete dictionary;
}

void GameEngine::init() {
    loadHighScore();
    // Đường dẫn này có thể cần sửa tùy vào nơi bạn chạy file thực thi
    // Để an toàn khi test, ta có thể set cứng hoặc truyền vào constructor
    dictionary = new Dictionary("tests/test_data/dict_test.txt"); 
    board = new Board();
    player = new Player();
}

TurnResult GameEngine::submitWord() {
    TurnResult result;
    result.success = false;
    result.scoreEarned = 0;

    // 1. Lấy từ trên bàn cờ
    WordPlacement placement = board->getPlacedWord();
    
    // 2. Kiểm tra vị trí đặt gạch có hợp lệ không (thẳng hàng, liền mạch)
    if (!placement.isValid) {
        recallAllTiles();
        result.message = "Invalid placement (not in a line or connected).";
        return result;
    }

    // 3. Kiểm tra từ điển
    if (dictionary->isValidWord(placement.word)) {
        // 4. Tính điểm
        int score = board->calculateScore(placement);
        player->addScore(score);
        
        // 5. Cập nhật trạng thái
        board->finalizeTurn();
        player->refillRack();
        
        // 6. Check Highscore
        if (player->getScore() > highScore) {
            highScore = player->getScore();
        }

        result.success = true;
        result.scoreEarned = score;
        result.message = "Valid Word: " + placement.word;
    } else {
        recallAllTiles();
        result.message = "Word not in dictionary: " + placement.word;
    }

    return result;
}

void GameEngine::resetLetters() {
    if (player->getLives() > 0) {
        recallAllTiles(); 
        player->resetRack();
        player->decreaseLife(); // Hàm này cần thêm vào Player.h/.cpp
    } 
    
    // Kiểm tra lại sau khi trừ mạng
    if (player->getLives() <= 0) {
        currentState = EngineState::GAME_OVER;
        saveHighScore();
    }
}

void GameEngine::recallAllTiles() {
    if (board && player) {
        board->recallTiles(player->getRack());
    }
}

void GameEngine::startOver() {
    saveHighScore();
    
    // Reset lại logic
    delete board;
    delete player;
    
    board = new Board();
    player = new Player();
    currentState = EngineState::PLAYING;
}

// --- Helper Functions ---

void GameEngine::loadHighScore() {
    std::ifstream file("assets/highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

void GameEngine::saveHighScore() {
    std::ofstream file("assets/highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}