// -- include/Constants.h --
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

// --- Screen and Layout ---
const int SCREEN_WIDTH = 1100; 
const int SCREEN_HEIGHT = 850; 
const int BOARD_DIMENSION = 15;
const int TILE_SIZE = 48;
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; 
const int BOARD_X_OFFSET = 40;
const int BOARD_Y_OFFSET = 40;
const int RACK_HEIGHT = 80;
const int RACK_Y_POS = BOARD_Y_OFFSET + BOARD_SIZE_PX + 20; 
const int PLAYER_RACK_SIZE = 8;
const int UI_PANEL_X = BOARD_X_OFFSET + BOARD_SIZE_PX + 40; 
const int MANUAL_ICON_SIZE = 50; // NEW: Defined size for the icon

// --- A generic color struct since SDL_Color is no longer in headers ---
struct Color { uint8_t r, g, b, a; };

// --- MODERN COLOR PALETTE ---
const Color COLOR_BACKGROUND = { 44, 62, 80, 255 };      
const Color COLOR_UI_PANEL = { 52, 73, 94, 255 };        
const Color COLOR_TILE_NORMAL = { 241, 196, 15, 255 };     
const Color COLOR_TILE_SHADOW = { 192, 57, 43, 255 };      
const Color COLOR_TILE_BORDER = { 0, 0, 0, 255 };         
const Color COLOR_TEXT_LIGHT = { 236, 240, 241, 255 };    
const Color COLOR_TEXT_DARK = { 44, 62, 80, 255 };         
const Color COLOR_BUTTON = { 26, 188, 156, 255 };         
const Color COLOR_BUTTON_HOVER = { 22, 160, 133, 255 };    
const Color COLOR_BUTTON_SHADOW = { 19, 118, 99, 255 };     
const Color COLOR_RACK_BG = { 44, 62, 80, 255 };          
const Color COLOR_GAMEOVER_BG = { 0, 0, 0, 220 }; 
const Color COLOR_GAMEOVER_DIALOG = { 52, 73, 94, 255 }; 
const Color COLOR_DBL_LETTER = { 52, 152, 219, 255 };     
const Color COLOR_TRP_LETTER = { 41, 128, 185, 255 };     
const Color COLOR_DBL_WORD = { 155, 89, 182, 255 };      
const Color COLOR_TRP_WORD = { 142, 68, 173, 255 };      
const Color COLOR_CENTER_STAR = { 243, 156, 18, 255 };    

// --- Game Data ---
const std::string FONT_PATH = "assets/montserrat.ttf";
const std::string DICTIONARY_PATH = "assets/dictionary.txt";
const std::string HIGHSCORE_PATH = "assets/highscore.txt";
const std::string IMAGE_ASSET_PATH = "assets/A-Z/"; 
const std::string MANUAL_ICON_PATH = "assets/manual_icon.png";
const int FONT_SIZE = 24;
const int FONT_SIZE_SMALL = 10;
const int FONT_SIZE_UI = 18;
const int FONT_SIZE_GAMEOVER = 48;

#endif // CONSTANTS_H