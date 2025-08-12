#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

using namespace std;

// --- Screen and Layout ---
const int SCREEN_WIDTH = 1100; 
const int SCREEN_HEIGHT = 950;
const int BOARD_DIMENSION = 15;
const int TILE_SIZE = 48;
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; // 720px
const int RACK_HEIGHT = 80;

// Dynamic Layout Calculation
const int UI_PANEL_WIDTH = 280;
const int GAME_AREA_WIDTH = SCREEN_WIDTH - UI_PANEL_WIDTH; // 1100 - 280 = 820px
const int HORIZONTAL_MARGIN = (GAME_AREA_WIDTH - BOARD_SIZE_PX) / 2; // (820 - 720) / 2 = 50px

const int BOARD_X_OFFSET = HORIZONTAL_MARGIN;
const int RACK_AREA_X = HORIZONTAL_MARGIN;
const int UI_PANEL_X = GAME_AREA_WIDTH;

// Y positions are now dynamic based on game mode
const int RACK_Y_POS_P1 = SCREEN_HEIGHT - RACK_HEIGHT - 20; // Player 1 (Bottom)
const int RACK_Y_POS_P2 = 20;                              // Player 2 (Top)
const int BOARD_Y_OFFSET_2P = RACK_Y_POS_P2 + RACK_HEIGHT + 20; // Y for 2-Player board
const int BOARD_Y_OFFSET_1P = 40; // Y for 1-Player board

const int PLAYER_RACK_SIZE = 7;
const int MANUAL_ICON_SIZE = 50;

struct Color { uint8_t r, g, b, a; };

// --- Main color ---
const Color COLOR_BACKGROUND = { 224, 224, 224, 255 };    
const Color COLOR_UI_PANEL = { 52, 73, 94, 255 };        
const Color COLOR_TILE_NORMAL = { 241, 196, 15, 255 };     
const Color COLOR_TILE_SHADOW = { 192, 57, 43, 255 };      
const Color COLOR_TILE_BORDER = { 0, 0, 0, 255 };         
const Color COLOR_TEXT_LIGHT = { 236, 240, 241, 255 };    
const Color COLOR_TEXT_DARK = { 44, 62, 80, 255 };         
const Color COLOR_BUTTON = { 26, 188, 156, 255 };         
const Color COLOR_BUTTON_HOVER = { 22, 160, 133, 255 };    
const Color COLOR_BUTTON_SHADOW = { 19, 118, 99, 255 };     
const Color COLOR_RACK_BG = { 199, 199, 199, 255 };         
const Color COLOR_GAMEOVER_BG = { 0, 0, 0, 220 }; 
const Color COLOR_GAMEOVER_DIALOG = { 52, 73, 94, 255 }; 
const Color COLOR_INACTIVE_OVERLAY = { 0, 0, 0, 100 }; 

// Bonus square colors
const Color COLOR_DBL_LETTER = { 212, 215, 250, 255 }; 
const Color COLOR_TRP_LETTER = { 105, 116, 250, 255 };  
const Color COLOR_DBL_WORD = { 255, 173, 175, 255 };    
const Color COLOR_TRP_WORD = { 252, 78, 83, 255 };      
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

#endif
