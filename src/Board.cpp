#include "Board.h"
#include "Constants.h" // Đảm bảo BOARD_DIMENSION có trong này
#include <algorithm>
#include <iostream>

// Constructor sạch: Khởi tạo mảng dữ liệu, không động vào đồ họa
Board::Board() {
    bonusGrid.resize(BOARD_DIMENSION, std::vector<Bonus>(BOARD_DIMENSION, NONE));
    tileGrid.resize(BOARD_DIMENSION, std::vector<Tile*>(BOARD_DIMENSION, nullptr));
    initializeBonusSquares();
}

Board::~Board() {
    // Không còn gì để cleanup vì vector tự quản lý bộ nhớ
}

// Hàm mới để bên ngoài lấy dữ liệu (cần cho Test và View sau này)
Bonus Board::getBonusAt(int row, int col) const {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION) return NONE;
    return bonusGrid[row][col];
}

Tile* Board::getTileAt(int row, int col) const {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION) return nullptr;
    return tileGrid[row][col];
}

void Board::placeTemporaryTile(Tile* tile, int row, int col) {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION || isOccupied(row, col)) return;
    
    // Nếu gạch đang ở chỗ khác trên bàn cờ, xóa chỗ cũ đi
    if(tile->boardRow != -1 && tile->boardRow < BOARD_DIMENSION) {
        tileGrid[tile->boardRow][tile->boardCol] = nullptr;
    }
    
    tileGrid[row][col] = tile;
    tile->boardRow = row;
    tile->boardCol = col;
    
    auto it = std::find(tempPlacedTiles.begin(), tempPlacedTiles.end(), tile);
    if(it == tempPlacedTiles.end()) {
        tempPlacedTiles.push_back(tile);
    }
}

bool Board::isOccupied(int row, int col) {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION) return true;
    return tileGrid[row][col] != nullptr;
}

WordPlacement Board::getPlacedWord() {
    WordPlacement result;
    if (tempPlacedTiles.empty()) return result;

    // Sắp xếp các gạch vừa đặt theo thứ tự trái->phải hoặc trên->dưới
    std::sort(tempPlacedTiles.begin(), tempPlacedTiles.end(), [](Tile* a, Tile* b) {
        if (a->boardRow != b->boardRow) return a->boardRow < b->boardRow;
        return a->boardCol < b->boardCol;
    });

    bool isHorizontal = true, isVertical = true;
    int firstRow = tempPlacedTiles[0]->boardRow;
    int firstCol = tempPlacedTiles[0]->boardCol;

    for (size_t i = 1; i < tempPlacedTiles.size(); ++i) {
        if (tempPlacedTiles[i]->boardRow != firstRow) isHorizontal = false;
        if (tempPlacedTiles[i]->boardCol != firstCol) isVertical = false;
    }

    if (!isHorizontal && !isVertical) { 
        result.isValid = false; 
        return result; 
    }

    result.tiles = tempPlacedTiles;
    for(const auto& tile : result.tiles) { 
        result.word += tile->getLetter(); 
    }
    result.isValid = true;
    return result;
}

void Board::recallTiles(std::vector<Tile*>& playerRack) {
    for (Tile* tile : tempPlacedTiles) {
        tileGrid[tile->boardRow][tile->boardCol] = nullptr;
        tile->boardRow = -1;
        tile->boardCol = -1;
        // Trả về rack tại vị trí cũ của nó
        if(tile->rackIndex >= 0 && tile->rackIndex < (int)playerRack.size()) {
            playerRack[tile->rackIndex] = tile;
        }
    }
    tempPlacedTiles.clear();
}

void Board::finalizeTurn() { 
    tempPlacedTiles.clear(); 
}

int Board::calculateScore(const WordPlacement& placement) {
    int totalScore = 0, wordMultiplier = 1;
    
    for(Tile* tile : placement.tiles) {
        int letterScore = tile->getValue();
        Bonus bonus = bonusGrid[tile->boardRow][tile->boardCol];
        
        switch(bonus) {
            case DOUBLE_LETTER: letterScore *= 2; break;
            case TRIPLE_LETTER: letterScore *= 3; break;
            case DOUBLE_WORD: case CENTER: wordMultiplier *= 2; break;
            case TRIPLE_WORD: wordMultiplier *= 3; break;
            default: break;
        }
        totalScore += letterScore;
    }
    return totalScore * wordMultiplier;
}

void Board::initializeBonusSquares() {
    // Reset toàn bộ về NONE
    for(auto& row : bonusGrid) std::fill(row.begin(), row.end(), NONE);

    bonusGrid[0][0] = bonusGrid[0][7] = bonusGrid[0][14] = TRIPLE_WORD;
    bonusGrid[7][0] = bonusGrid[7][14] = TRIPLE_WORD;
    bonusGrid[14][0] = bonusGrid[14][7] = bonusGrid[14][14] = TRIPLE_WORD;

    for (int i = 1; i < 5; ++i) {
        bonusGrid[i][i] = bonusGrid[i][14 - i] = DOUBLE_WORD;
        bonusGrid[14 - i][i] = bonusGrid[14 - i][14 - i] = DOUBLE_WORD;
    }

    bonusGrid[7][7] = CENTER; // Tính là Double Word

    bonusGrid[1][5] = bonusGrid[1][9] = TRIPLE_LETTER;
    bonusGrid[5][1] = bonusGrid[5][5] = bonusGrid[5][9] = bonusGrid[5][13] = TRIPLE_LETTER;
    bonusGrid[9][1] = bonusGrid[9][5] = bonusGrid[9][9] = bonusGrid[9][13] = TRIPLE_LETTER;
    bonusGrid[13][5] = bonusGrid[13][9] = TRIPLE_LETTER;

    bonusGrid[0][3] = bonusGrid[0][11] = DOUBLE_LETTER;
    bonusGrid[2][6] = bonusGrid[2][8] = DOUBLE_LETTER;
    bonusGrid[3][0] = bonusGrid[3][7] = bonusGrid[3][14] = DOUBLE_LETTER;
    bonusGrid[6][2] = bonusGrid[6][6] = bonusGrid[6][8] = bonusGrid[6][12] = DOUBLE_LETTER;
    bonusGrid[7][3] = bonusGrid[7][11] = DOUBLE_LETTER;
    bonusGrid[8][2] = bonusGrid[8][6] = bonusGrid[8][8] = bonusGrid[8][12] = DOUBLE_LETTER;
    bonusGrid[11][0] = bonusGrid[11][7] = bonusGrid[11][14] = DOUBLE_LETTER;
    bonusGrid[12][6] = bonusGrid[12][8] = DOUBLE_LETTER;
    bonusGrid[14][3] = bonusGrid[14][11] = DOUBLE_LETTER;
}