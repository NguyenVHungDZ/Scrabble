// -- include/Constants.h --
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

// --- Screen and Layout ---
const int SCREEN_WIDTH = 1100; // WIDER screen for UI panel
const int SCREEN_HEIGHT = 850; // Shorter screen
const int BOARD_DIMENSION = 15;
const int TILE_SIZE = 48;
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; // 720px
const int BOARD_X_OFFSET = 40;
const int BOARD_Y_OFFSET = 40;
const int RACK_HEIGHT = 80;
const int RACK_Y_POS = BOARD_Y_OFFSET + BOARD_SIZE_PX + 20; // Rack is now below the board
const int PLAYER_RACK_SIZE = 8;
const int UI_PANEL_X = BOARD_X_OFFSET + BOARD_SIZE_PX + 40; // UI starts to the right of the board

// --- A generic color struct since SDL_Color is no longer in headers ---
struct Color { uint8_t r, g, b, a; };
const Color COLOR_BOARD_BG = {0, 70, 70, 255};
const Color COLOR_TILE_NORMAL = {245, 245, 220, 255};
const Color COLOR_TILE_BORDER = {210, 180, 140, 255};
const Color COLOR_RACK_BG = {139, 69, 19, 255};
const Color COLOR_TEXT = {0, 0, 0, 255};
const Color COLOR_BUTTON = {70, 130, 180, 255};
const Color COLOR_BUTTON_TEXT = {255, 255, 255, 255};
const Color COLOR_GAMEOVER_BG = {0, 0, 0, 200}; 
const Color COLOR_GAMEOVER_DIALOG = {40, 40, 60, 255}; 
const Color COLOR_DBL_LETTER = {173, 216, 230, 255};
const Color COLOR_TRP_LETTER = {0, 0, 139, 255};
const Color COLOR_DBL_WORD = {255, 182, 193, 255};
const Color COLOR_TRP_WORD = {255, 0, 0, 255};
const Color COLOR_CENTER_STAR = {255, 215, 0, 255};

// --- Game Data ---
const std::string FONT_PATH = "assets/arial.ttf";
const std::string DICTIONARY_PATH = "assets/dictionary.txt";
const std::string HIGHSCORE_PATH = "assets/highscore.txt";
const int FONT_SIZE = 24;
const int FONT_SIZE_SMALL = 10;
const int FONT_SIZE_UI = 18;
const int FONT_SIZE_GAMEOVER = 48;

#endif // CONSTANTS_H