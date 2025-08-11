#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

using namespace std;

const int SCREEN_WIDTH = 1100; 
const int SCREEN_HEIGHT = 850; 
const int BOARD_DIMENSION = 15; // 15 x 15 board
const int TILE_SIZE = 48; // Each tile is 48x48 pixels
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; // Board size in pixels
const int BOARD_X_OFFSET = 40; // Board top margin
const int BOARD_Y_OFFSET = 40; // Board left margin
const int RACK_HEIGHT = 80;
const int RACK_Y_POS = BOARD_Y_OFFSET + BOARD_SIZE_PX + 20; //
const int PLAYER_RACK_SIZE = 7;
const int UI_PANEL_X = BOARD_X_OFFSET + BOARD_SIZE_PX + 40; 
const int MANUAL_ICON_SIZE = 50; // Guide icon's size

struct Color { uint8_t r, g, b, a; };

// --- Main color ---
const Color COLOR_BACKGROUND = { 224, 224, 224, 255 }; //background     
const Color COLOR_UI_PANEL = { 52, 73, 94, 255 };        
const Color COLOR_TILE_NORMAL = { 241, 196, 15, 255 };     
const Color COLOR_TILE_SHADOW = { 192, 57, 43, 255 };      
const Color COLOR_TILE_BORDER = { 0, 0, 0, 255 };         
const Color COLOR_TEXT_LIGHT = { 236, 240, 241, 255 };    
const Color COLOR_TEXT_DARK = { 44, 62, 80, 255 };         
const Color COLOR_BUTTON = { 26, 188, 156, 255 };         
const Color COLOR_BUTTON_HOVER = { 22, 160, 133, 255 };    
const Color COLOR_BUTTON_SHADOW = { 19, 118, 99, 255 };     
const Color COLOR_RACK_BG = { 199, 199, 199, 255 }; // Grey          
const Color COLOR_GAMEOVER_BG = { 0, 0, 0, 220 }; 
const Color COLOR_GAMEOVER_DIALOG = { 52, 73, 94, 255 }; 
const Color COLOR_DBL_LETTER = { 212, 215, 250, 255 }; // Light blue     
const Color COLOR_TRP_LETTER = { 105, 116, 250, 255 }; // Dark blue   
const Color COLOR_DBL_WORD = { 255, 173, 175, 255 }; // Light red      
const Color COLOR_TRP_WORD = { 252, 78, 83, 255 }; // Dark red      
const Color COLOR_CENTER_STAR = { 243, 156, 18, 255 }; // Yellow    

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

#endif 