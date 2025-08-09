// -- include/Board.h --
#ifndef BOARD_H
#define BOARD_H
#include <vector>
#include <string>
#include "Tile.h" 

using namespace std;

struct SDL_Renderer;

enum Bonus { NONE, DOUBLE_LETTER, TRIPLE_LETTER, DOUBLE_WORD, TRIPLE_WORD, CENTER };
struct WordPlacement {
    string word;
    vector<Tile*> tiles;
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
    void recallTiles(vector<Tile*>& playerRack);
    void finalizeTurn();
    int calculateScore(const WordPlacement& placement);
private:
    void initializeBonusSquares();
    void renderBonusSquare(int row, int col);
    SDL_Renderer* renderer;
    vector<vector<Bonus>> bonusGrid;
    vector<vector<Tile*>> tileGrid;
    vector<Tile*> tempPlacedTiles;
};
#endif // BOARD_H