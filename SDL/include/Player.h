// -- include/Player.h --
#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string>
#include <map>
#include "Tile.h"

// Forward declarations
struct SDL_Renderer;
typedef struct TTF_Font TTF_Font;

class Player {
public:
    Player(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont);
    ~Player();
    void renderRack();
    void refillRack();
    void resetRack(); // NEW: Function to reset letters
    
    std::vector<Tile*>& getRack() { return rack; }
    Tile* getTileFromRack(int index);
    void removeTileFromRack(int index);
    void returnTileToRack(Tile* tile);
    
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    int getLives() const { return lives; } // NEW: Getter for lives

private:
    void initializeTileBag();
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* smallFont;
    int score;
    int lives; // NEW: Player lives
    std::vector<Tile*> rack;
    std::vector<char> tileBag;
    std::map<char, int> tileValues;
};
#endif // PLAYER_H