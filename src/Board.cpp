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

/**
 * @brief Hàm khởi tạo (Constructor) cho lớp Board.
 * * @param renderer Con trỏ đến SDL_Renderer để vẽ.
 * @param font Con trỏ đến TTF_Font để vẽ chữ lên các ô thưởng.
 */
Board::Board(SDL_Renderer* renderer, TTF_Font* font) 
    : renderer(renderer), textFont(font) {
    // Khởi tạo lưới chứa thông tin về các ô thưởng (bonus)
    bonusGrid.resize(BOARD_DIMENSION, vector<Bonus>(BOARD_DIMENSION, NONE));
    // Khởi tạo lưới chứa con trỏ đến các quân cờ (tile), ban đầu tất cả là nullptr (ô trống)
    tileGrid.resize(BOARD_DIMENSION, vector<Tile*>(BOARD_DIMENSION, nullptr));
    // Gọi hàm để thiết lập vị trí các ô thưởng theo luật Scrabble
    initializeBonusSquares();
}

/**
 * @brief Hàm hủy (Destructor) cho lớp Board.
 * Dọn dẹp các quân cờ còn lại trên bàn cờ để tránh rò rỉ bộ nhớ.
 */
Board::~Board() {
    // Lặp qua từng ô trên bàn cờ
    for (int row = 0; row < BOARD_DIMENSION; ++row) {
        for (int col = 0; col < BOARD_DIMENSION; ++col) {
            // Nếu ô có chứa một quân cờ
            if (tileGrid[row][col] != nullptr) {
                // Kiểm tra xem quân cờ này có phải là quân cờ tạm thời không
                // Điều này để tránh xóa một con trỏ hai lần nếu game kết thúc đột ngột
                bool isTemp = false;
                for(const auto& tempTile : tempPlacedTiles) {
                    if (tileGrid[row][col] == tempTile) {
                        isTemp = true;
                        break;
                    }
                }
                // Chỉ xóa nếu nó không phải là quân cờ tạm thời (đã được xác nhận từ lượt trước)
                if (!isTemp) {
                    delete tileGrid[row][col];
                }
            }
        }
    }
}

/**
 * @brief Vẽ toàn bộ bàn cờ lên màn hình.
 * Bao gồm các ô thưởng, lưới và các quân cờ đã được đặt.
 */
void Board::render() {
    for (int row = 0; row < BOARD_DIMENSION; ++row) {
        for (int col = 0; col < BOARD_DIMENSION; ++col) {
            // 1. Vẽ nền cho các ô (màu đặc biệt cho ô thưởng, hoặc trống)
            renderBonusSquare(row, col);

            // 2. Vẽ các đường lưới
            SDL_Rect squareRect = { BOARD_X_OFFSET + col * TILE_SIZE, BOARD_Y_OFFSET + row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_SetRenderDrawColor(renderer, 52, 73, 94, 100); // Màu xám mờ cho lưới
            SDL_RenderDrawRect(renderer, &squareRect);

            // 3. Nếu có quân cờ trên ô này, vẽ nó ra
            if (tileGrid[row][col] != nullptr) {
                tileGrid[row][col]->render(squareRect.x, squareRect.y, false, -1, -1);
            }
        }
    }
}

/**
 * @brief Đặt một quân cờ lên bàn cờ một cách tạm thời.
 * Quân cờ này chưa được xác nhận và có thể được thu hồi.
 * * @param tile Con trỏ đến quân cờ được đặt.
 * @param row Hàng trên bàn cờ.
 * @param col Cột trên bàn cờ.
 */
void Board::placeTemporaryTile(Tile* tile, int row, int col) {
    // Kiểm tra xem vị trí có hợp lệ và chưa bị chiếm không
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION || isOccupied(row, col)) return;
    
    // Nếu quân cờ này đã được đặt ở một vị trí khác trên bàn, hãy xóa nó khỏi vị trí cũ
    if (tile->boardRow != -1 && tile->boardCol != -1) {
        tileGrid[tile->boardRow][tile->boardCol] = nullptr;
    }

    // Đặt quân cờ vào vị trí mới trên lưới
    tileGrid[row][col] = tile;
    tile->boardRow = row;
    tile->boardCol = col;

    // Thêm quân cờ vào danh sách các quân cờ tạm thời nếu nó chưa có trong danh sách
    auto it = find(tempPlacedTiles.begin(), tempPlacedTiles.end(), tile);
    if (it == tempPlacedTiles.end()) {
        tempPlacedTiles.push_back(tile);
    }
}

/**
 * @brief Kiểm tra xem một ô trên bàn cờ đã có quân cờ hay chưa.
 * * @param row Hàng cần kiểm tra.
 * @param col Cột cần kiểm tra.
 * @return true nếu ô đã bị chiếm, false nếu ô còn trống.
 */
bool Board::isOccupied(int row, int col) {
    if (row < 0 || row >= BOARD_DIMENSION || col < 0 || col >= BOARD_DIMENSION) return true;
    return tileGrid[row][col] != nullptr;
}

/**
 * @brief Tìm tất cả các từ mới được tạo ra bởi các quân cờ tạm thời.
 * Đây là logic cốt lõi để xác định các từ hợp lệ trong một lượt đi.
 * * @return vector<WordPlacement> Một danh sách các từ mới được tìm thấy.
 */
vector<WordPlacement> Board::findAllNewWords() {
    vector<WordPlacement> foundWords;
    if (tempPlacedTiles.empty()) {
        return foundWords; // Nếu không có quân cờ nào được đặt, trả về danh sách rỗng
    }

    // Sử dụng std::set để đảm bảo mỗi từ chỉ được thêm vào một lần
    set<string> distinctWords;

    // Sắp xếp các quân cờ tạm thời theo hàng và cột để dễ dàng xác định hướng đi
    sort(tempPlacedTiles.begin(), tempPlacedTiles.end(), [](const Tile* a, const Tile* b) {
        if (a->boardRow != b->boardRow) return a->boardRow < b->boardRow;
        return a->boardCol < b->boardCol;
    });

    // Xác định hướng chính của nước đi (ngang hay dọc)
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
        // Nếu chỉ có 1 quân cờ được đặt, nó có thể tạo từ theo cả hai hướng
        isHorizontal = true;
        isVertical = true;
    }

    // --- LOGIC TÌM TỪ CHÍNH (MAIN WORD) ---

    // Tìm từ chính theo hướng ngang
    if (isHorizontal) {
        Tile* startTile = tempPlacedTiles.front();
        int row = startTile->boardRow;
        int startCol = startTile->boardCol;

        // Lùi về bên trái để tìm điểm bắt đầu của từ
        while (startCol > 0 && tileGrid[row][startCol - 1] != nullptr) {
            startCol--;
        }

        // Đọc từ trái sang phải để tạo thành từ hoàn chỉnh
        WordPlacement mainWord;
        int currentCol = startCol;
        while (currentCol < BOARD_DIMENSION && tileGrid[row][currentCol] != nullptr) {
            Tile* currentTile = tileGrid[row][currentCol];
            mainWord.word += currentTile->getLetter();
            mainWord.tiles.push_back(currentTile);
            currentCol++;
        }

        // Chỉ thêm vào nếu từ có nhiều hơn 1 chữ cái và chưa tồn tại
        if (mainWord.word.length() > 1 && distinctWords.find(mainWord.word) == distinctWords.end()) {
            foundWords.push_back(mainWord);
            distinctWords.insert(mainWord.word);
        }
    }
    
    // Tìm từ chính theo hướng dọc (logic tương tự hướng ngang)
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

        if (mainWord.word.length() > 1 && distinctWords.find(mainWord.word) == distinctWords.end()) {
            foundWords.push_back(mainWord);
            distinctWords.insert(mainWord.word);
        }
    }

    // --- LOGIC TÌM TỪ PHỤ (SECONDARY WORDS) ---
    // Các từ phụ được tạo ra vuông góc với hướng đi chính
    for (Tile* newTile : tempPlacedTiles) {
        // Nếu đi ngang, tìm từ phụ theo chiều dọc tại mỗi quân cờ mới
        if (isHorizontal) {
            int startRow = newTile->boardRow;
            int col = newTile->boardCol;
            while (startRow > 0 && tileGrid[startRow - 1][col] != nullptr) startRow--;
            
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
        // Nếu đi dọc, tìm từ phụ theo chiều ngang tại mỗi quân cờ mới
        if (isVertical) {
            int row = newTile->boardRow;
            int startCol = newTile->boardCol;
            while (startCol > 0 && tileGrid[row][startCol - 1] != nullptr) startCol--;

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

/**
 * @brief Thu hồi tất cả các quân cờ tạm thời từ bàn cờ về lại khay của người chơi.
 * * @param playerRack Vector các quân cờ trên khay của người chơi.
 */
void Board::recallTiles(vector<Tile*>& playerRack) {
    for (Tile* tile : tempPlacedTiles) {
        // Xóa con trỏ khỏi lưới bàn cờ
        if (tile->boardRow != -1 && tile->boardCol != -1) {
            tileGrid[tile->boardRow][tile->boardCol] = nullptr;
        }
        // Reset lại tọa độ của quân cờ
        tile->boardRow = -1;
        tile->boardCol = -1;
        // Đặt quân cờ trở lại đúng vị trí cũ trên khay
        playerRack[tile->rackIndex] = tile;
    }
    // Xóa sạch danh sách các quân cờ tạm thời
    tempPlacedTiles.clear();
}

/**
 * @brief Hoàn tất lượt đi.
 * Các quân cờ tạm thời bây giờ trở thành một phần của bàn cờ.
 */
void Board::finalizeTurn() {
    // Chỉ cần xóa danh sách các quân cờ tạm thời. Chúng vẫn còn trên tileGrid.
    tempPlacedTiles.clear();
}

/**
 * @brief Tính điểm cho một từ (WordPlacement).
 * Logic này tuân theo luật Scrabble: chỉ áp dụng điểm thưởng cho các quân cờ mới.
 * * @param placement Từ cần tính điểm.
 * @return int Tổng số điểm của từ đó.
 */
int Board::calculateScore(const WordPlacement& placement) {
    int wordScore = 0;
    int wordMultiplier = 1;

    // Lặp qua từng quân cờ trong từ
    for (Tile* tile : placement.tiles) {
        int letterScore = tile->getValue();

        // Kiểm tra xem quân cờ này có phải là quân cờ mới được đặt trong lượt này không
        bool isNewTile = false;
        for (Tile* tempTile : tempPlacedTiles) {
            if (tile == tempTile) {
                isNewTile = true;
                break;
            }
        }
        
        // Chỉ áp dụng điểm thưởng (bonus) nếu đây là quân cờ mới
        if (isNewTile) {
            Bonus bonus = bonusGrid[tile->boardRow][tile->boardCol];
            switch (bonus) {
                case DOUBLE_LETTER: letterScore *= 2; break; // Nhân đôi điểm chữ cái
                case TRIPLE_LETTER: letterScore *= 3; break; // Nhân ba điểm chữ cái
                case DOUBLE_WORD:   // Nhân đôi điểm từ
                case CENTER:        // Ô trung tâm cũng là nhân đôi điểm từ
                    wordMultiplier *= 2; 
                    break;
                case TRIPLE_WORD: wordMultiplier *= 3; break; // Nhân ba điểm từ
                default: break;
            }
        }
        // Cộng điểm của chữ cái vào tổng điểm của từ
        wordScore += letterScore;
    }

    // Áp dụng hệ số nhân từ cho toàn bộ từ
    return wordScore * wordMultiplier;
}

/**
 * @brief Thiết lập vị trí của các ô thưởng trên bàn cờ.
 */
void Board::initializeBonusSquares() {
    // TRIPLE_WORD (x3 điểm từ)
    bonusGrid[0][0] = bonusGrid[0][7] = bonusGrid[0][14] = TRIPLE_WORD;
    bonusGrid[7][0] = bonusGrid[7][14] = TRIPLE_WORD;
    bonusGrid[14][0] = bonusGrid[14][7] = bonusGrid[14][14] = TRIPLE_WORD;

    // DOUBLE_WORD (x2 điểm từ)
    for (int i = 1; i < 5; ++i) {
        bonusGrid[i][i] = bonusGrid[i][14 - i] = DOUBLE_WORD;
        bonusGrid[14 - i][i] = bonusGrid[14 - i][14 - i] = DOUBLE_WORD;
    }
    bonusGrid[7][7] = CENTER; // Ô trung tâm

    // TRIPLE_LETTER (x3 điểm chữ cái)
    bonusGrid[1][5] = bonusGrid[1][9] = TRIPLE_LETTER;
    bonusGrid[5][1] = bonusGrid[5][5] = bonusGrid[5][9] = bonusGrid[5][13] = TRIPLE_LETTER;
    bonusGrid[9][1] = bonusGrid[9][5] = bonusGrid[9][9] = bonusGrid[9][13] = TRIPLE_LETTER;
    bonusGrid[13][5] = bonusGrid[13][9] = TRIPLE_LETTER;

    // DOUBLE_LETTER (x2 điểm chữ cái)
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

/**
 * @brief Vẽ một ô thưởng (nền màu và chữ).
 * Hàm này chỉ được gọi cho các ô trống.
 * * @param row Hàng của ô.
 * @param col Cột của ô.
 */
void Board::renderBonusSquare(int row, int col) {
    // Nếu ô đã có quân cờ thì không làm gì cả
    if (tileGrid[row][col] != nullptr) return;

    SDL_Rect squareRect = { BOARD_X_OFFSET + col * TILE_SIZE, BOARD_Y_OFFSET + row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
    const Color* c = nullptr;
    string bonusText = "";

    // Xác định màu và chữ cho từng loại ô thưởng
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
            bonusText = "★"; // Sử dụng ký tự ngôi sao cho ô trung tâm
            break;
        default: 
            // Đây là ô trống bình thường, không cần vẽ gì đặc biệt
            return;
    }

    // Vẽ màu nền cho ô
    SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, c->a);
    SDL_RenderFillRect(renderer, &squareRect);

    // Vẽ chữ lên trên nền
    if (!bonusText.empty() && textFont) {
        // Tạo texture từ chuỗi văn bản
        SDL_Texture* textTexture = TextureManager::LoadText(renderer, textFont, bonusText, COLOR_TEXT_LIGHT);
        if (textTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

            // Tính toán vị trí để căn chữ ra giữa ô
            SDL_Rect destRect = {
                squareRect.x + (TILE_SIZE - textWidth) / 2,
                squareRect.y + (TILE_SIZE - textHeight) / 2,
                textWidth,
                textHeight
            };

            // Vẽ texture chữ
            SDL_RenderCopy(renderer, textTexture, NULL, &destRect);
            
            // Hủy texture sau khi vẽ để tránh rò rỉ bộ nhớ
            SDL_DestroyTexture(textTexture);
        }
    }
}
