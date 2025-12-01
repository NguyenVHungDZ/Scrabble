#ifndef TILE_H
#define TILE_H

class Tile {
public:
    // Constructor không còn SDL_Renderer hay Font
    Tile(char letter, int value);
    ~Tile();

    char getLetter() const { return letter; }
    int getValue() const { return value; }

    // Dữ liệu vị trí (Logic)
    int rackIndex;
    int boardRow, boardCol;

    // XÓA HẾT các biến SDL_Texture, SDL_Renderer

private:
    char letter;
    int value;
};
#endif // TILE_H