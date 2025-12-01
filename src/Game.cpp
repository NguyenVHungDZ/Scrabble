#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h> 
#include <SDL_ttf.h>
#include "Game.h"
#include "Constants.h"
#include "TextureManager.h"
#include <iostream>
#include <string>

Game::Game() 
    : isRunning(false), window(nullptr), renderer(nullptr), 
      mainFont(nullptr), smallFont(nullptr), uiFont(nullptr), gameOverFont(nullptr), 
      manualIconTexture(nullptr), selectedTile(nullptr), mouseX(0), mouseY(0) {}

Game::~Game() { cleanup(); }

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    window = SDL_CreateWindow("Scrabble (Clean Arch)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Load Fonts
    mainFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE);
    smallFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_SMALL);
    uiFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_UI);
    gameOverFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_GAMEOVER);

    // Khởi tạo Logic Game
    engine.init(); 

    // Setup UI Rects
    submitButtonRect = { UI_PANEL_X, 40, 220, 50 };
    recallButtonRect = { UI_PANEL_X, 110, 220, 50 };
    resetButtonRect = { UI_PANEL_X, 180, 220, 50 }; 
    startOverButtonRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50 };
    manualButtonRect = { SCREEN_WIDTH - (MANUAL_ICON_SIZE + 20), SCREEN_HEIGHT - (MANUAL_ICON_SIZE + 20), MANUAL_ICON_SIZE, MANUAL_ICON_SIZE };
    
    manualIconTexture = TextureManager::LoadImage(renderer, MANUAL_ICON_PATH);

    isRunning = true;
    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        render(); // Update và Render gộp làm 1 vì logic update nằm ở Engine rồi
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) isRunning = false;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            // Xử lý nút bấm UI
            if (engine.getState() == EngineState::PLAYING) {
                if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &submitButtonRect)) {
                    TurnResult res = engine.submitWord();
                    std::cout << res.message << std::endl; // Log kết quả
                }
                else if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &recallButtonRect)) {
                    engine.recallAllTiles();
                }
                else if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &resetButtonRect)) {
                    engine.resetLetters();
                }
                
                // Xử lý gắp gạch từ khay (Rack)
                int rackStartX = BOARD_X_OFFSET + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
                int rackY = RACK_Y_POS + (RACK_HEIGHT - TILE_SIZE) / 2;
                auto& rack = engine.player->getRack();
                
                for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
                    SDL_Rect tileRect = { rackStartX + i * (TILE_SIZE + 5), rackY, TILE_SIZE, TILE_SIZE };
                    if (rack[i] && SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &tileRect)) {
                        selectedTile = rack[i];
                        engine.player->removeTileFromRack(i); // Logic: Gạch rời khỏi khay
                        break;
                    }
                }
            } 
            else if (engine.getState() == EngineState::GAME_OVER) {
                if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &startOverButtonRect)) {
                    engine.startOver();
                }
            }
        }
        
        // Thả chuột -> Đặt gạch
        if (e.type == SDL_MOUSEBUTTONUP && selectedTile) {
            int col = (mouseX - BOARD_X_OFFSET) / TILE_SIZE;
            int row = (mouseY - BOARD_Y_OFFSET) / TILE_SIZE;
            
            // Gọi Engine để check logic đặt
            if (row >= 0 && row < BOARD_DIMENSION && col >= 0 && col < BOARD_DIMENSION && !engine.board->isOccupied(row, col)) {
                engine.board->placeTemporaryTile(selectedTile, row, col);
            } else {
                // Đặt sai chỗ -> Trả về khay
                engine.player->returnTileToRack(selectedTile);
            }
            selectedTile = nullptr;
        }
    }
}

// --- PHẦN VẼ (RENDER) ---
// Vì Tile không còn hàm render(), Game phải tự vẽ Tile

void Game::drawTile(Tile* tile, int x, int y, bool isDragging) {
    if (!tile) return;
    
    SDL_Rect dest = { x, y, TILE_SIZE, TILE_SIZE };
    
    // 1. Vẽ nền gạch
    SDL_SetRenderDrawColor(renderer, 245, 245, 220, 255); // Màu kem
    if (isDragging) SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255); // Sáng hơn khi kéo
    SDL_RenderFillRect(renderer, &dest);
    
    // 2. Vẽ viền
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &dest);

    // 3. Vẽ Chữ cái (Load ảnh hoặc Text)
    // Cách tối ưu: Nên cache texture, nhưng để đơn giản ta load text trực tiếp
    std::string letterStr(1, tile->getLetter());
    SDL_Texture* tex = TextureManager::LoadText(renderer, mainFont, letterStr, {0,0,0,255});
    if (tex) {
        int w, h; SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        SDL_Rect textDest = { x + (TILE_SIZE - w)/2, y + (TILE_SIZE - h)/2 - 5, w, h };
        SDL_RenderCopy(renderer, tex, NULL, &textDest);
        SDL_DestroyTexture(tex);
    }
    
    // 4. Vẽ điểm số nhỏ
    std::string valStr = std::to_string(tile->getValue());
    SDL_Texture* valTex = TextureManager::LoadText(renderer, smallFont, valStr, {0,0,0,255});
    if (valTex) {
        int w, h; SDL_QueryTexture(valTex, NULL, NULL, &w, &h);
        SDL_Rect valDest = { x + TILE_SIZE - w - 2, y + TILE_SIZE - h - 2, w, h };
        SDL_RenderCopy(renderer, valTex, NULL, &valDest);
        SDL_DestroyTexture(valTex);
    }
}

void Game::renderBoard() {
    for (int r = 0; r < BOARD_DIMENSION; ++r) {
        for (int c = 0; c < BOARD_DIMENSION; ++c) {
            int x = BOARD_X_OFFSET + c * TILE_SIZE;
            int y = BOARD_Y_OFFSET + r * TILE_SIZE;
            SDL_Rect rect = {x, y, TILE_SIZE, TILE_SIZE};

            // Vẽ ô bonus (Logic lấy từ Engine)
            Bonus b = engine.board->getBonusAt(r, c);
            if (b == TRIPLE_WORD) SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
            else if (b == DOUBLE_WORD) SDL_SetRenderDrawColor(renderer, 255, 150, 150, 255);
            else if (b == TRIPLE_LETTER) SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255);
            else if (b == DOUBLE_LETTER) SDL_SetRenderDrawColor(renderer, 150, 150, 255, 255);
            else if (b == CENTER) SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255);
            else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ô thường
            
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Viền ô
            SDL_RenderDrawRect(renderer, &rect);

            // Vẽ gạch nếu có
            Tile* t = engine.board->getTileAt(r, c);
            if (t) drawTile(t, x, y, false);
        }
    }
}

void Game::render() {
    // Xóa màn hình
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderClear(renderer);

    renderBoard();

    // Render Rack
    int rackStartX = BOARD_X_OFFSET + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
    int rackY = RACK_Y_POS + (RACK_HEIGHT - TILE_SIZE) / 2;
    auto& rack = engine.player->getRack();
    
    for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
        if (rack[i]) {
            drawTile(rack[i], rackStartX + i * (TILE_SIZE + 5), rackY, false);
        }
    }

    renderUI();

    // Vẽ viên gạch đang kéo (nằm trên cùng)
    if (selectedTile) {
        drawTile(selectedTile, mouseX - TILE_SIZE/2, mouseY - TILE_SIZE/2, true);
    }
    
    if (engine.getState() == EngineState::GAME_OVER) renderGameOver();
    // if (engine.getState() == MANUAL) renderManual();

    SDL_RenderPresent(renderer);
}

// ... (Giữ nguyên các hàm renderUI, renderGameOver, renderManual, cleanup cũ) ...
// CHÚ Ý: Trong renderUI, thay đổi cách lấy điểm: engine.player->getScore()
void Game::renderUI() {
    // ... Copy code renderUI cũ vào đây ...
    // Sửa đoạn lấy text:
    // std::string scoreText = "Score: " + std::to_string(engine.player->getScore());
    // std::string livesText = "Lives: " + std::to_string(engine.player->getLives());
}

void Game::cleanup() {
    // Hủy tài nguyên SDL
    if (mainFont) TTF_CloseFont(mainFont);
    // ...
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
// Các hàm còn lại giữ nguyên logic vẽ (chỉ thay đổi nguồn dữ liệu lấy từ engine)