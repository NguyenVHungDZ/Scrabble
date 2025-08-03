// -- include/Constants.h --
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> // Required for uint8_t

// --- Screen and Layout ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 920;
const int BOARD_DIMENSION = 15;
const int TILE_SIZE = 48;
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE;
const int BOARD_X_OFFSET = (SCREEN_WIDTH - BOARD_SIZE_PX) / 2;
const int BOARD_Y_OFFSET = 20;
const int RACK_HEIGHT = 80;
const int RACK_Y_POS = SCREEN_HEIGHT - RACK_HEIGHT - 80;
const int PLAYER_RACK_SIZE = 8; 
const int UI_AREA_Y = RACK_Y_POS + RACK_HEIGHT + 10;

// --- A generic color struct since SDL_Color is no longer in headers ---
struct Color { uint8_t r, g, b, a; };
const Color COLOR_BOARD_BG = {0, 70, 70, 255};
const Color COLOR_TILE_NORMAL = {245, 245, 220, 255};
const Color COLOR_TILE_BORDER = {210, 180, 140, 255};
const Color COLOR_RACK_BG = {139, 69, 19, 255};
const Color COLOR_TEXT = {0, 0, 0, 255};
const Color COLOR_BUTTON = {70, 130, 180, 255};
const Color COLOR_BUTTON_TEXT = {255, 255, 255, 255};
const Color COLOR_DBL_LETTER = {173, 216, 230, 255};
const Color COLOR_TRP_LETTER = {0, 0, 139, 255};
const Color COLOR_DBL_WORD = {255, 182, 193, 255};
const Color COLOR_TRP_WORD = {255, 0, 0, 255};
const Color COLOR_CENTER_STAR = {255, 215, 0, 255};

// --- Game Data ---
const std::string FONT_PATH = "assets/arial.ttf";
const std::string DICTIONARY_PATH = "assets/dictionary.txt";
const int FONT_SIZE = 24;
const int FONT_SIZE_SMALL = 10;
const int FONT_SIZE_UI = 18;

#endif // CONSTANTS_H