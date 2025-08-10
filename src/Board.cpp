// -- src/Board.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Board.h"
#include "Constants.h"
#include "TextureManager.h"
#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

// Constructor đã được cập nhật để nhận font
Board::Board(SDL_Renderer* renderer, TTF_Font* font) 
    : renderer(renderer), textFont(font) {
    bonusGrid.resize(BOARD_DIMENSION, vector<Bonus>(BOARD_DIMENSION, NONE));
    tileGrid.resize(BOARD_DIMENSION, vector<Tile*>(BOARD_DIMENSION, nullptr));
    initializeBonusSquares();
}

Board::~Board() {
    // Dọn dẹp các tile còn lại trên bàn cờ khi Board bị hủy
    for (int row = 0; row < BOARD_DIMENSION; ++row) {
        for (int col = 0; col < BOARD_DIMENSION; ++col) {
            if (tileGrid[row][col] != nullptr) {
                // Chỉ xóa nếu nó không còn nằm trong danh sách tạm thời
                // (tránh xóa kép nếu game kết thúc đột ngột)
                bool isTemp = false;
                for(const auto& tempTile : tempPlacedTiles) {
                    if (tileGrid[row][col] == tempTile) {
                        isTemp = true;
                        break;
                    }
                }
                if (!isTemp) {
                    delete tileGrid[row][col];
                }
            }
        }
    }
}

void Board::render() {
    for (int row = 0; row < BOARD_DIMENSION; ++row) {
        for (int col = 0; col < BOARD_DIMENSION; ++col) {
            // Vẽ nền ô bonus hoặc ô trống
            renderBonusSquare(row, col);

            // Vẽ lưới
            SDL_Rect squareRect = { BOARD_X_OFFSET + col * TILE_SIZE, BOARD_Y_OFFSET + row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_SetRenderDrawColor(renderer, 52, 73, 94, 100); 
            SDL_RenderDrawRect(renderer, &squareRect);

            // Vẽ quân cờ nếu có
            if (tileGrid[row][col] != nullptr) {
                tileGrid[row][col]->render(squareRect.x, squareRect.y, false, -1, -1);
            }
        }
    }
}

void Board::placeTemporaryTile(Tile* tile, int row, int col) {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION || isOccupied(row, col)) return;
    
    // Nếu tile này đã được đặt ở đâu đó trên bàn cờ, hãy xóa nó khỏi vị trí cũ
    if (tile->boardRow != -1 && tile->boardCol != -1) {
        tileGrid[tile->boardRow][tile->boardCol] = nullptr;
    }

    tileGrid[row][col] = tile;
    tile->boardRow = row;
    tile->boardCol = col;

    // Đảm bảo tile chỉ được thêm vào danh sách tạm thời một lần
    auto it = find(tempPlacedTiles.begin(), tempPlacedTiles.end(), tile);
    if (it == tempPlacedTiles.end()) {
        tempPlacedTiles.push_back(tile);
    }
}

bool Board::isOccupied(int row, int col) {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION) return true;
    return tileGrid[row][col] != nullptr;
}

vector<WordPlacement> Board::findAllNewWords() {
    vector<WordPlacement> foundWords;
    if (tempPlacedTiles.empty()) {
        return foundWords;
    }

    set<string> distinctWords; // Dùng set để tránh thêm từ trùng lặp

    // Sắp xếp các quân cờ tạm thời để dễ xác định hướng
    sort(tempPlacedTiles.begin(), tempPlacedTiles.end(), [](const Tile* a, const Tile* b) {
        if (a->boardRow != b->boardRow) return a->boardRow < b->boardRow;
        return a->boardCol < b->boardCol;
    });

    // Xác định hướng chính của các quân cờ đã đặt
    bool isHorizontal = true;
    bool isVertical = true;
    if (tempPlacedTiles.size() > 1) {
        int firstRow = tempPlacedTiles.front()->boardRow;
        int firstCol = tempPlacedTiles.front()->boardCol;
        for (size_t i = 1; i < tempPlacedTiles.size(); ++i) {
            if (tempPlacedTiles[i]->boardRow != firstRow) isHorizontal = false;
            if (tempPlacedTiles[i]->boardCol != firstCol) isVertical = false;
        }
    } else {
        // Nếu chỉ có 1 quân cờ, nó có thể tạo từ theo cả hai hướng
        isHorizontal = true;
        isVertical = true;
    }

    // Tìm từ chính theo hướng ngang
    if (isHorizontal) {
        Tile* startTile = tempPlacedTiles.front();
        int row = startTile->boardRow;
        int startCol = startTile->boardCol;

        while (startCol > 0 && tileGrid[row][startCol - 1] != nullptr) {
            startCol--;
        }

        WordPlacement mainWord;
        int currentCol = startCol;
        while (currentCol < BOARD_DIMENSION && tileGrid[row][currentCol] != nullptr) {
            Tile* currentTile = tileGrid[row][currentCol];
            mainWord.word += currentTile->getLetter();
            mainWord.tiles.push_back(currentTile);
            currentCol++;
        }

        if (mainWord.word.length() > 1) {
            if (distinctWords.find(mainWord.word) == distinctWords.end()) {
                foundWords.push_back(mainWord);
                distinctWords.insert(mainWord.word);
            }
        }
    }
    
    // Tìm từ chính theo hướng dọc
    if (isVertical) {
        Tile* startTile = tempPlacedTiles.front();
        int startRow = startTile->boardRow;
        int col = startTile->boardCol;

        while (startRow > 0 && tileGrid[startRow - 1][col] != nullptr) {
            startRow--;
        }

        WordPlacement mainWord;
        int currentRow = startRow;
        while (currentRow < BOARD_DIMENSION && tileGrid[currentRow][col] != nullptr) {
            Tile* currentTile = tileGrid[currentRow][col];
            mainWord.word += currentTile->getLetter();
            mainWord.tiles.push_back(currentTile);
            currentRow++;
        }

        if (mainWord.word.length() > 1) {
            if (distinctWords.find(mainWord.word) == distinctWords.end()) {
                foundWords.push_back(mainWord);
                distinctWords.insert(mainWord.word);
            }
        }
    }

    // Tìm các từ phụ (vuông góc) cho mỗi quân cờ mới
    for (Tile* newTile : tempPlacedTiles) {
        // Tìm từ dọc nếu hướng chính là ngang
        if (isHorizontal) {
            int startRow = newTile->boardRow;
            int col = newTile->boardCol;
            while (startRow > 0 && tileGrid[startRow - 1][col] != nullptr) {
                startRow--;
            }
            WordPlacement secondaryWord;
            int currentRow = startRow;
            while (currentRow < BOARD_DIMENSION && tileGrid[currentRow][col] != nullptr) {
                secondaryWord.word += tileGrid[currentRow][col]->getLetter();
                secondaryWord.tiles.push_back(tileGrid[currentRow][col]);
                currentRow++;
            }
            if (secondaryWord.word.length() > 1 && distinctWords.find(secondaryWord.word) == distinctWords.end()) {
                foundWords.push_back(secondaryWord);
                distinctWords.insert(secondaryWord.word);
            }
        }
        // Tìm từ ngang nếu hướng chính là dọc
        if (isVertical) {
            int row = newTile->boardRow;
            int startCol = newTile->boardCol;
            while (startCol > 0 && tileGrid[row][startCol - 1] != nullptr) {
                startCol--;
            }
            WordPlacement secondaryWord;
            int currentCol = startCol;
            while (currentCol < BOARD_DIMENSION && tileGrid[row][currentCol] != nullptr) {
                secondaryWord.word += tileGrid[row][currentCol]->getLetter();
                secondaryWord.tiles.push_back(tileGrid[row][currentCol]);
                currentCol++;
            }
            if (secondaryWord.word.length() > 1 && distinctWords.find(secondaryWord.word) == distinctWords.end()) {
                foundWords.push_back(secondaryWord);
                distinctWords.insert(secondaryWord.word);
            }
        }
    }

    return foundWords;
}

void Board::recallTiles(vector<Tile*>& playerRack) {
    for (Tile* tile : tempPlacedTiles) {
        if (tile->boardRow != -1 && tile->boardCol != -1) {
            tileGrid[tile->boardRow][tile->boardCol] = nullptr;
        }
        tile->boardRow = -1;
        tile->boardCol = -1;
        playerRack[tile->rackIndex] = tile; // Trả về đúng vị trí trên khay
    }
    tempPlacedTiles.clear();
}

void Board::finalizeTurn() {
    tempPlacedTiles.clear();
}

// Hàm tính điểm đã được sửa lại
int Board::calculateScore(const WordPlacement& placement) {
    int wordScore = 0;
    int wordMultiplier = 1;

    for (Tile* tile : placement.tiles) {
        int letterScore = tile->getValue();

        // Kiểm tra xem tile này có phải là tile mới được đặt không
        bool isNewTile = false;
        for (Tile* tempTile : tempPlacedTiles) {
            if (tile == tempTile) {
                isNewTile = true;
                break;
            }
        }
        
        // Chỉ áp dụng bonus nếu là tile mới
        if (isNewTile) {
            Bonus bonus = bonusGrid[tile->boardRow][tile->boardCol];
            switch (bonus) {
                case DOUBLE_LETTER: letterScore *= 2; break;
                case TRIPLE_LETTER: letterScore *= 3; break;
                case DOUBLE_WORD: case CENTER: wordMultiplier *= 2; break;
                case TRIPLE_WORD: wordMultiplier *= 3; break;
                default: break;
            }
        }
        wordScore += letterScore;
    }

    return wordScore * wordMultiplier;
}

void Board::initializeBonusSquares() {
    bonusGrid[0][0] = bonusGrid[0][7] = bonusGrid[0][14] = TRIPLE_WORD;
    bonusGrid[7][0] = bonusGrid[7][14] = TRIPLE_WORD;
    bonusGrid[14][0] = bonusGrid[14][7] = bonusGrid[14][14] = TRIPLE_WORD;

    for (int i = 1; i < 5; ++i) {
        bonusGrid[i][i] = bonusGrid[i][14 - i] = DOUBLE_WORD;
        bonusGrid[14 - i][i] = bonusGrid[14 - i][14 - i] = DOUBLE_WORD;
    }
    bonusGrid[7][7] = CENTER;

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

// Hàm vẽ ô bonus đã được cập nhật để vẽ chữ
void Board::renderBonusSquare(int row, int col) {
    if (tileGrid[row][col] != nullptr) return;

    SDL_Rect squareRect = { BOARD_X_OFFSET + col * TILE_SIZE, BOARD_Y_OFFSET + row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    const Color* c = nullptr;
    string bonusText = "";

    switch (bonusGrid[row][col]) {
        case DOUBLE_LETTER: 
            c = &COLOR_DBL_LETTER;
            bonusText = "DL";
            break;
        case TRIPLE_LETTER: 
            c = &COLOR_TRP_LETTER;
            bonusText = "TL";
            break;
        case DOUBLE_WORD:   
            c = &COLOR_DBL_WORD;
            bonusText = "DW";
            break;
        case TRIPLE_WORD:   
            c = &COLOR_TRP_WORD;
            bonusText = "TW";
            break;
        case CENTER:        
            c = &COLOR_CENTER_STAR;
            bonusText = "Start";
            break;
        default: 
            return;
    }

    // Vẽ màu nền cho ô
    SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, c->a);
    SDL_RenderFillRect(renderer, &squareRect);

    // Vẽ chữ lên trên
    if (!bonusText.empty() && textFont) {
        SDL_Texture* textTexture = TextureManager::LoadText(renderer, textFont, bonusText, COLOR_TEXT_LIGHT);
        if (textTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect destRect = {
                squareRect.x + (TILE_SIZE - textWidth) / 2,
                squareRect.y + (TILE_SIZE - textHeight) / 2,
                textWidth,
                textHeight
            };

            SDL_RenderCopy(renderer, textTexture, NULL, &destRect);
            SDL_DestroyTexture(textTexture);
        }
    }
}
