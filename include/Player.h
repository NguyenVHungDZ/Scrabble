#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <map>
#include "Tile.h"

using namespace std;

class Player {
public:
    // Constructor sạch
    Player();
    ~Player();

    // Các hàm Logic thuần túy (TEST ĐƯỢC)
    void refillRack();
    void resetRack(); 
    
    vector<Tile*>& getRack() { return rack; }
    Tile* getTileFromRack(int index);
    void removeTileFromRack(int index);
    void returnTileToRack(Tile* tile);
    
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    int getLives() const { return lives; } 
    void decreaseLife() { if(lives > 0) lives--; }

    // XÓA hàm renderRack()

private:
    void initializeTileBag();
    
    int score;
    int lives; 
    vector<Tile*> rack;
    
    // Logic túi bài
    vector<char> tileBag;
    map<char, int> tileValues;
};
#endif // PLAYER_H