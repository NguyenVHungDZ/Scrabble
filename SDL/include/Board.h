// -- include/Board.h --
#ifndef BOARD_H
#define BOARD_H
#include <vector>
#include <string>
#include "Tile.h" 

// Forward declaration
struct SDL_Renderer;

enum Bonus { NONE, DOUBLE_LETTER, TRIPLE_LETTER, DOUBLE_WORD, TRIPLE_WORD, CENTER };
struct WordPlacement {
    std::string word;
    std::vector<Tile*> tiles;
    bool isValid = false;
};
class Board {
public:
    Board(SDL_Renderer* renderer);
    ~Board();
    void render();
    void placeTemporaryTile(Tile* tile, int row, int col);
    bool isOccupied(int row, int col);
    WordPlacement getPlacedWord();
    void recallTiles(std::vector<Tile*>& playerRack);
    void finalizeTurn();
    int calculateScore(const WordPlacement& placement);
private:
    void initializeBonusSquares();
    void renderBonusSquare(int row, int col);
    SDL_Renderer* renderer;
    std::vector<std::vector<Bonus>> bonusGrid;
    std::vector<std::vector<Tile*>> tileGrid;
    std::vector<Tile*> tempPlacedTiles;
};
#endif // BOARD_H