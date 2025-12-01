#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include "Tile.h" 

// 1. XÓA dòng này: struct SDL_Renderer; 

using namespace std;

enum Bonus { NONE, DOUBLE_LETTER, TRIPLE_LETTER, DOUBLE_WORD, TRIPLE_WORD, CENTER };

struct WordPlacement {
    string word;
    vector<Tile*> tiles;
    bool isValid = false;
};

class Board {
public:
    // 2. Sửa Constructor: Không nhận renderer nữa
    Board(); 
    ~Board();

    // 3. XÓA hàm vẽ: void render(); -> Chuyển ra ngoài class

    // Logic giữ nguyên (Test được)
    void placeTemporaryTile(Tile* tile, int row, int col);
    bool isOccupied(int row, int col);
    WordPlacement getPlacedWord();
    void recallTiles(vector<Tile*>& playerRack);
    void finalizeTurn();
    int calculateScore(const WordPlacement& placement);

    // 4. THÊM Getter: Để bên ngoài lấy dữ liệu mà vẽ
    Bonus getBonusAt(int row, int col) const;
    Tile* getTileAt(int row, int col) const;

private:
    void initializeBonusSquares();
    
    // 5. XÓA hàm vẽ nội bộ: void renderBonusSquare(int row, int col);
    
    // 6. XÓA biến renderer: SDL_Renderer* renderer;

    vector<vector<Bonus>> bonusGrid;
    vector<vector<Tile*>> tileGrid;
    vector<Tile*> tempPlacedTiles;
};

#endif // BOARD_H