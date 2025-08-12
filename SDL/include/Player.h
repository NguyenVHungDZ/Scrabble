// -- include/Player.h --
#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string>
#include <map>
#include "Tile.h"

using namespace std;

struct SDL_Renderer;
typedef struct TTF_Font TTF_Font;

class Player {
public:
    Player(string name, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont);
    ~Player();

    void renderRack(int mouseX, int mouseY, int yPos, bool isActive);
    void refillRack(vector<char>& tileBag, const map<char, int>& tileValues);
    void resetRack(vector<char>& tileBag);

    vector<Tile*>& getRack() { return rack; }
    Tile* getTileFromRack(int index);
    void removeTileFromRack(int index);
    void returnTileToRack(Tile* tile);

    string getName() const { return name; }
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    void setScore(int newScore) { score = newScore; }
    int getLives() const { return lives; }
    void useLife() { if (lives > 0) lives--; }
    bool hasTiles() const;

private:
    string name;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* smallFont;
    int score;
    int lives;
    vector<Tile*> rack;
};
#endif
