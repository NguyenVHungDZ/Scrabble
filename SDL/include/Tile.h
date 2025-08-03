// -- include/Tile.h --
#ifndef TILE_H
#define TILE_H
#include <string>

// Forward declarations instead of including SDL headers
struct SDL_Renderer;
struct SDL_Texture;
typedef struct TTF_Font TTF_Font;

class Tile {
public:
    Tile(char letter, int value, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont);
    ~Tile();
    void render(int x, int y);
    char getLetter() const { return letter; }
    int getValue() const { return value; }
    int rackIndex;
    int boardRow, boardCol;
private:
    char letter;
    int value;
    SDL_Renderer* renderer;
    SDL_Texture* letterTexture;
    SDL_Texture* valueTexture;
};
#endif // TILE_H