// -- src/Game.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Game.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Board.h"
#include "Player.h"
#include "Dictionary.h"
#include <iostream>

Game::Game() 
    : isRunning(false), window(nullptr), renderer(nullptr), 
      mainFont(nullptr), smallFont(nullptr), uiFont(nullptr),
      board(nullptr), player(nullptr), dictionary(nullptr),
      selectedTile(nullptr), mouseX(0), mouseY(0) {}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    std::cout << "Initializing game..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "ERROR: SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "ERROR: SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "ERROR: Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "ERROR: Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    mainFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE);
    smallFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_SMALL);
    uiFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_UI);
    if (!mainFont || !smallFont || !uiFont) {
        std::cerr << "ERROR: Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    dictionary = new Dictionary(DICTIONARY_PATH);
    board = new Board(renderer);
    player = new Player(renderer, mainFont, smallFont);
    
    // Define all UI button rectangles
    submitButtonRect = { BOARD_X_OFFSET, UI_AREA_Y, 150, 40 };
    recallButtonRect = { BOARD_X_OFFSET + 160, UI_AREA_Y, 150, 40 };
    resetButtonRect = { BOARD_X_OFFSET + 320, UI_AREA_Y, 150, 40 }; 

    isRunning = true;
    std::cout << "Initialization complete. Starting game loop." << std::endl;
    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) { isRunning = false; }
        SDL_GetMouseState(&mouseX, &mouseY);
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            // Check for UI button clicks
            if (mouseX >= submitButtonRect.x && mouseX < submitButtonRect.x + submitButtonRect.w &&
                mouseY >= submitButtonRect.y && mouseY < submitButtonRect.y + submitButtonRect.h) {
                submitWord();
                continue;
            }
            if (mouseX >= recallButtonRect.x && mouseX < recallButtonRect.x + recallButtonRect.w &&
                mouseY >= recallButtonRect.y && mouseY < recallButtonRect.y + recallButtonRect.h) {
                recallAllTiles();
                continue;
            }
            if (mouseX >= resetButtonRect.x && mouseX < resetButtonRect.x + resetButtonRect.w &&
                mouseY >= resetButtonRect.y && mouseY < resetButtonRect.y + resetButtonRect.h) {
                resetLetters();
                continue;
            }

            // Check for tile clicks
            int rackStartX = BOARD_X_OFFSET + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
            int rackY = RACK_Y_POS + (RACK_HEIGHT - TILE_SIZE) / 2;
            for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
                SDL_Rect tileRect = { rackStartX + i * (TILE_SIZE + 5), rackY, TILE_SIZE, TILE_SIZE };
                if (player->getTileFromRack(i) && mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    selectedTile = player->getTileFromRack(i);
                    player->removeTileFromRack(i);
                    break;
                }
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (selectedTile) {
                int boardCol = (mouseX - BOARD_X_OFFSET) / TILE_SIZE;
                int boardRow = (mouseY - BOARD_Y_OFFSET) / TILE_SIZE;
                if (boardRow >= 0 && boardRow < BOARD_DIMENSION && boardCol >= 0 && boardCol < BOARD_DIMENSION && !board->isOccupied(boardRow, boardCol)) {
                    board->placeTemporaryTile(selectedTile, boardRow, boardCol);
                } else {
                    player->returnTileToRack(selectedTile);
                }
                selectedTile = nullptr;
            }
        }
    }
}

void Game::submitWord() {
    WordPlacement placement = board->getPlacedWord();
    if (!placement.isValid) {
        std::cout << "Invalid placement." << std::endl;
        recallAllTiles();
        return;
    }
    if (dictionary->isValidWord(placement.word)) {
        int score = board->calculateScore(placement);
        player->addScore(score);
        std::cout << "Word '" << placement.word << "' is valid! Score: " << score << std::endl;
        board->finalizeTurn();
        player->refillRack();
    } else {
        std::cout << "Word '" << placement.word << "' is not in the dictionary." << std::endl;
        recallAllTiles();
    }
}

void Game::recallAllTiles() {
    board->recallTiles(player->getRack());
}

void Game::resetLetters() {
    if (player->getLives() > 0) {
        std::cout << "Player used a reset. " << (player->getLives() - 1) << " resets left." << std::endl;
        recallAllTiles(); 
        player->resetRack();
    } else {
        std::cout << "No resets left!" << std::endl;
    }
}

void Game::update() {}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, COLOR_BOARD_BG.r, COLOR_BOARD_BG.g, COLOR_BOARD_BG.b, COLOR_BOARD_BG.a);
    SDL_RenderClear(renderer);
    board->render();
    player->renderRack();
    renderUI();
    if (selectedTile) {
        selectedTile->render(mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2);
    }
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    // --- Draw Button Backgrounds ---
    SDL_SetRenderDrawColor(renderer, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, COLOR_BUTTON.a);
    SDL_RenderFillRect(renderer, &submitButtonRect);
    SDL_RenderFillRect(renderer, &recallButtonRect);
    SDL_RenderFillRect(renderer, &resetButtonRect);

    // --- Draw Submit Button Text ---
    SDL_Texture* submitTexture = TextureManager::LoadText(renderer, uiFont, "Submit Word", COLOR_BUTTON_TEXT);
    if (submitTexture) {
        int w, h;
        SDL_QueryTexture(submitTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { submitButtonRect.x + (submitButtonRect.w - w) / 2, submitButtonRect.y + (submitButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, submitTexture, NULL, &dest);
        SDL_DestroyTexture(submitTexture);
    }

    // --- Draw Recall Button Text ---
    SDL_Texture* recallTexture = TextureManager::LoadText(renderer, uiFont, "Recall Tiles", COLOR_BUTTON_TEXT);
    if (recallTexture) {
        int w, h;
        SDL_QueryTexture(recallTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { recallButtonRect.x + (recallButtonRect.w - w) / 2, recallButtonRect.y + (recallButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, recallTexture, NULL, &dest);
        SDL_DestroyTexture(recallTexture);
    }
    
    // --- Draw Reset Button Text ---
    SDL_Texture* resetTexture = TextureManager::LoadText(renderer, uiFont, "Reset Letters", COLOR_BUTTON_TEXT);
    if (resetTexture) {
        int w, h;
        SDL_QueryTexture(resetTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { resetButtonRect.x + (resetButtonRect.w - w) / 2, resetButtonRect.y + (resetButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, resetTexture, NULL, &dest);
        SDL_DestroyTexture(resetTexture);
    }

    // --- Draw Score and Lives Text ---
    std::string scoreText = "Score: " + std::to_string(player->getScore());
    std::string livesText = "Resets Left: " + std::to_string(player->getLives());

    SDL_Texture* scoreTexture = TextureManager::LoadText(renderer, uiFont, scoreText, COLOR_BUTTON_TEXT);
    if(scoreTexture) {
        int w, h;
        SDL_QueryTexture(scoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH - w - 40, UI_AREA_Y + 5, w, h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
        SDL_DestroyTexture(scoreTexture);
    }
    SDL_Texture* livesTexture = TextureManager::LoadText(renderer, uiFont, livesText, COLOR_BUTTON_TEXT);
    if(livesTexture) {
        int w, h;
        SDL_QueryTexture(livesTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH - w - 40, UI_AREA_Y + 25, w, h};
        SDL_RenderCopy(renderer, livesTexture, NULL, &dest);
        SDL_DestroyTexture(livesTexture);
    }
}

void Game::cleanup() {
    delete board;
    delete player;
    delete dictionary;
    if (mainFont) TTF_CloseFont(mainFont);
    if (smallFont) TTF_CloseFont(smallFont);
    if (uiFont) TTF_CloseFont(uiFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}