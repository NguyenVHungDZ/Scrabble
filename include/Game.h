#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "GameEngine.h" // Nhúng bộ não Logic vào
#include "TextureManager.h"

class Game {
public:
    Game();
    ~Game();

    // Khởi tạo SDL, Window, Renderer và Logic Engine
    bool init();
    
    // Vòng lặp chính của game
    void run();

private:
    // --- CONTROLLER (Xử lý Input) ---
    void handleEvents();
    
    // --- VIEW (Xử lý Hiển thị) ---
    void render();
    void cleanup();

    // Các hàm helper để vẽ từng phần
    void renderUI();
    void renderBoard();     // Lấy dữ liệu từ engine.board để vẽ
    void renderGameOver();
    void renderManual();
    
    // HÀM MỚI: Game phải tự vẽ gạch vì Tile.cpp không còn code vẽ nữa
    void drawTile(Tile* tile, int x, int y, bool isDragging);

    // --- MEMBER VARIABLES ---
    bool isRunning;
    
    // 1. Logic Core (Model)
    GameEngine engine; 

    // 2. SDL Resources (View)
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Fonts
    TTF_Font* mainFont; 
    TTF_Font* smallFont; 
    TTF_Font* uiFont;
    TTF_Font* gameOverFont; 
    
    // Textures
    SDL_Texture* manualIconTexture; 
    
    // 3. Input State (Trạng thái chuột/kéo thả)
    Tile* selectedTile; // Con trỏ trỏ đến viên gạch đang được kéo (thuộc về engine)
    int mouseX, mouseY;
    
    // 4. UI Layout (Vị trí các nút bấm)
    SDL_Rect submitButtonRect; 
    SDL_Rect recallButtonRect;
    SDL_Rect resetButtonRect;
    SDL_Rect startOverButtonRect; 
    SDL_Rect manualButtonRect; 
};

#endif // GAME_H