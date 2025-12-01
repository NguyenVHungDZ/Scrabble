#include "Tile.h"

// Constructor: Chỉ khởi tạo giá trị, KHÔNG load ảnh, KHÔNG dùng SDL
Tile::Tile(char letter, int value) 
    : letter(letter), value(value), 
      rackIndex(-1), boardRow(-1), boardCol(-1) 
{
}

// Destructor: Không còn texture để xóa
Tile::~Tile() {}