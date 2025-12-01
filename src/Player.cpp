#include "Player.h"
#include "Constants.h" // Đảm bảo có PLAYER_RACK_SIZE
#include <random>
#include <algorithm>
#include <iostream>

// Constructor sạch: Khởi tạo điểm, mạng và túi bài
Player::Player() : score(0), lives(3) { 
    initializeTileBag();
    rack.resize(PLAYER_RACK_SIZE, nullptr);
    refillRack();
}

Player::~Player() {
    for (Tile* tile : rack) { 
        if(tile) delete tile; 
    }
    rack.clear();
}

void Player::refillRack() {
    for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
        // Chỉ thêm gạch nếu ô đó trống và túi còn bài
        if (rack[i] == nullptr && !tileBag.empty()) {
            char letter = tileBag.back();
            tileBag.pop_back();
            
            // Lấy giá trị từ map
            int value = tileValues[letter];
            
            // TẠO TILE MỚI (Không còn SDL renderer/font ở đây)
            rack[i] = new Tile(letter, value);
            rack[i]->rackIndex = i;
        }
    }
}

void Player::resetRack() {
    if (lives <= 0) return; 
    
    lives--; // Trừ mạng (Logic game)
    
    // Trả lại bài vào túi (trừ quân bài trắng nếu muốn)
    for (size_t i = 0; i < rack.size(); ++i) {
        if (rack[i] != nullptr) {
            // Logic cũ của bạn: trả lại túi
            tileBag.push_back(rack[i]->getLetter());
            
            delete rack[i];
            rack[i] = nullptr;
        }
    }
    
    // Xáo trộn túi
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tileBag.begin(), tileBag.end(), g);
    
    // Bốc lại
    refillRack();
}

// Hàm renderRack đã bị XÓA. Việc vẽ sẽ do Game.cpp thực hiện dựa trên dữ liệu rack.

Tile* Player::getTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) return rack[index];
    return nullptr;
}

void Player::removeTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) { 
        // Lưu ý: Không delete tile ở đây, vì tile này đang được chuột kéo đi nơi khác
        rack[index] = nullptr; 
    }
}

void Player::returnTileToRack(Tile* tile) {
    if (tile && tile->rackIndex >= 0 && (size_t)tile->rackIndex < rack.size()) {
        rack[tile->rackIndex] = tile;
        tile->boardRow = -1;
        tile->boardCol = -1;
    }
}

void Player::initializeTileBag() {
    tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}, {' ', 0}
    };
    std::map<char, int> tileCounts = {
        {'A', 9}, {'B', 2}, {'C', 2}, {'D', 4}, {'E', 12}, {'F', 2}, {'G', 3},
        {'H', 2}, {'I', 9}, {'J', 1}, {'K', 1}, {'L', 4}, {'M', 2}, {'N', 6},
        {'O', 8}, {'P', 2}, {'Q', 1}, {'R', 6}, {'S', 4}, {'T', 6}, {'U', 4},
        {'V', 2}, {'W', 2}, {'X', 1}, {'Y', 2}, {'Z', 1}, {' ', 2}
    };
    for (auto const& [letter, count] : tileCounts) {
        for (int i = 0; i < count; ++i) tileBag.push_back(letter);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tileBag.begin(), tileBag.end(), g);
}