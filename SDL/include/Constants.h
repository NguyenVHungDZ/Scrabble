#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint> 

using namespace std;

// --- Kích thước màn hình và Bố cục ---
const int SCREEN_WIDTH = 1100; // Chiều rộng cửa sổ game
const int SCREEN_HEIGHT = 950; // Chiều cao cửa sổ game (đã tăng để chứa 2 khay)
const int BOARD_DIMENSION = 15; // Kích thước bàn cờ (15x15 ô)
const int TILE_SIZE = 48; // Kích thước mỗi ô cờ (pixel)
const int BOARD_SIZE_PX = BOARD_DIMENSION * TILE_SIZE; // Kích thước bàn cờ (pixel)
const int RACK_HEIGHT = 80; // Chiều cao của khay chứa quân cờ

// Tính toán bố cục động
const int UI_PANEL_WIDTH = 280; // Chiều rộng của bảng điều khiển bên phải
const int GAME_AREA_WIDTH = SCREEN_WIDTH - UI_PANEL_WIDTH; // Chiều rộng khu vực chơi game
const int HORIZONTAL_MARGIN = (GAME_AREA_WIDTH - BOARD_SIZE_PX) / 2; // Căn lề ngang để bàn cờ ra giữa

const int BOARD_X_OFFSET = HORIZONTAL_MARGIN; // Vị trí X của bàn cờ và khay
const int RACK_AREA_X = HORIZONTAL_MARGIN;
const int UI_PANEL_X = GAME_AREA_WIDTH; // Vị trí X của bảng điều khiển

// Vị trí Y (chiều dọc) cho các thành phần
const int RACK_Y_POS_P1 = SCREEN_HEIGHT - RACK_HEIGHT - 20; // Vị trí Y khay người chơi 1 (dưới)
const int RACK_Y_POS_P2 = 20;                              // Vị trí Y khay người chơi 2 (trên)
const int BOARD_Y_OFFSET_2P = RACK_Y_POS_P2 + RACK_HEIGHT + 20; // Vị trí Y bàn cờ khi chơi 2 người
const int BOARD_Y_OFFSET_1P = 40; // Vị trí Y bàn cờ khi chơi 1 người

const int PLAYER_RACK_SIZE = 7; // Số lượng quân cờ tối đa trên khay
const int MANUAL_ICON_SIZE = 50; // Kích thước icon hướng dẫn

// Cấu trúc để lưu trữ màu sắc RGBA
struct Color { uint8_t r, g, b, a; };

// --- Bảng màu chính ---
const Color COLOR_BACKGROUND = { 224, 224, 224, 255 };    // Màu nền chính
const Color COLOR_UI_PANEL = { 52, 73, 94, 255 };        // Màu bảng điều khiển
const Color COLOR_TILE_NORMAL = { 241, 196, 15, 255 };     // Màu quân cờ
const Color COLOR_TILE_SHADOW = { 192, 57, 43, 255 };      // Màu bóng của quân cờ
const Color COLOR_TILE_BORDER = { 0, 0, 0, 255 };         // Màu viền quân cờ
const Color COLOR_TEXT_LIGHT = { 236, 240, 241, 255 };    // Màu chữ sáng
const Color COLOR_TEXT_DARK = { 44, 62, 80, 255 };         // Màu chữ tối
const Color COLOR_BUTTON = { 26, 188, 156, 255 };         // Màu nút bấm
const Color COLOR_BUTTON_HOVER = { 22, 160, 133, 255 };    // Màu nút bấm khi di chuột qua
const Color COLOR_BUTTON_SHADOW = { 19, 118, 99, 255 };     // Màu bóng của nút
const Color COLOR_RACK_BG = { 199, 199, 199, 255 };         // Màu nền khay cờ
const Color COLOR_GAMEOVER_BG = { 0, 0, 0, 220 };         // Màu lớp phủ khi game over
const Color COLOR_GAMEOVER_DIALOG = { 52, 73, 94, 255 }; // Màu hộp thoại game over
const Color COLOR_INACTIVE_OVERLAY = { 0, 0, 0, 100 };   // Màu lớp phủ "làm mờ" khay không hoạt động

// Màu các ô thưởng
const Color COLOR_DBL_LETTER = { 212, 215, 250, 255 }; // Double Letter
const Color COLOR_TRP_LETTER = { 105, 116, 250, 255 }; // Triple Letter
const Color COLOR_DBL_WORD = { 255, 173, 175, 255 };   // Double Word
const Color COLOR_TRP_WORD = { 252, 78, 83, 255 };     // Triple Word
const Color COLOR_CENTER_STAR = { 243, 156, 18, 255 };  // Ô trung tâm

// --- Đường dẫn đến các tệp dữ liệu ---
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
