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

// --- NEW MODERN COLOR PALETTE ---
const Color COLOR_BACKGROUND = { 44, 62, 80, 255 };      // Dark Slate
const Color COLOR_UI_PANEL = { 52, 73, 94, 255 };        // Lighter Slate
const Color COLOR_TILE_NORMAL = { 241, 196, 15, 255 };     // Sunflower Yellow
const Color COLOR_TILE_SHADOW = { 192, 57, 43, 255 };      // Dark Orange/Red for shadow
const Color COLOR_TEXT_LIGHT = { 236, 240, 241, 255 };    // Off-White
const Color COLOR_TEXT_DARK = { 44, 62, 80, 255 };         // Dark Slate for on-tile text
const Color COLOR_BUTTON = { 26, 188, 156, 255 };         // Teal
const Color COLOR_BUTTON_HOVER = { 22, 160, 133, 255 };    // Darker Teal
const Color COLOR_BUTTON_SHADOW = { 19, 118, 99, 255 };     // Even Darker Teal
const Color COLOR_RACK_BG = { 44, 62, 80, 255 };          // Same as background for seamless look
const Color COLOR_GAMEOVER_BG = { 0, 0, 0, 220 }; 
const Color COLOR_GAMEOVER_DIALOG = { 52, 73, 94, 255 }; 
const Color COLOR_DBL_LETTER = { 52, 152, 219, 255 };     // Peter River Blue
const Color COLOR_TRP_LETTER = { 41, 128, 185, 255 };     // Belize Hole Blue
const Color COLOR_DBL_WORD = { 155, 89, 182, 255 };      // Amethyst Purple
const Color COLOR_TRP_WORD = { 142, 68, 173, 255 };      // Wisteria Purple
const Color COLOR_CENTER_STAR = { 243, 156, 18, 255 };    // Orange

// --- Game Data ---
const std::string FONT_PATH = "assets/arial.ttf";
const std::string DICTIONARY_PATH = "assets/dictionary.txt";
const std::string HIGHSCORE_PATH = "assets/highscore.txt";
const int FONT_SIZE = 24;
const int FONT_SIZE_SMALL = 10;
const int FONT_SIZE_UI = 18;
const int FONT_SIZE_GAMEOVER = 48;

#endif // CONSTANTS_H