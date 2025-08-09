// -- include/Constants.h --
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

using namespace std;

// --- Screen and Layout ---
const int SCREEN_WIDTH = 1100; 
const int SCREEN_HEIGHT = 850; 
const int BOARD_DIMENSION = 15; // 15 x 15 board
const int TILE_SIZE = 48; // Each tile is 48x48 pixels
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; // Board size in pixels
const int BOARD_X_OFFSET = 40; // Board top margin
const int BOARD_Y_OFFSET = 40; // Board left margin
const int RACK_HEIGHT = 80;
const int RACK_Y_POS = BOARD_Y_OFFSET + BOARD_SIZE_PX + 20; 
const int PLAYER_RACK_SIZE = 8;
const int UI_PANEL_X = BOARD_X_OFFSET + BOARD_SIZE_PX + 40; 
const int MANUAL_ICON_SIZE = 50; // Guide icon's size

// --- A generic color ---
struct Color { uint8_t r, g, b, a; };

// --- MODERN COLOR PALETTE ---
const Color COLOR_BACKGROUND = { 44, 62, 80, 255 }; //background     
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

// --- Game Data's Paths ---
const string FONT_PATH = "assets/montserrat.ttf";
const string DICTIONARY_PATH = "assets/dictionary.txt";
const string HIGHSCORE_PATH = "assets/highscore.txt";
const string IMAGE_ASSET_PATH = "assets/A-Z/"; 
const string MANUAL_ICON_PATH = "assets/manual_icon.png";
const int FONT_SIZE = 24;
const int FONT_SIZE_SMALL = 10;
const int FONT_SIZE_UI = 18;
const int FONT_SIZE_GAMEOVER = 48;

#endif // CONSTANTS_H