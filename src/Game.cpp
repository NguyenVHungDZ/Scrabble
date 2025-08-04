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
#include <fstream>

Game::Game() 
    : isRunning(false), currentState(PLAYING), highScore(0), window(nullptr), renderer(nullptr), 
      mainFont(nullptr), smallFont(nullptr), uiFont(nullptr), gameOverFont(nullptr),
      board(nullptr), player(nullptr), dictionary(nullptr),
      selectedTile(nullptr), mouseX(0), mouseY(0) {}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) { return false; }
    window = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) { return false; }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { return false; }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    mainFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE);
    smallFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_SMALL);
    uiFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_UI);
    gameOverFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_GAMEOVER);
    if (!mainFont || !smallFont || !uiFont || !gameOverFont) {
        std::cerr << "ERROR: Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    loadHighScore();
    dictionary = new Dictionary(DICTIONARY_PATH);
    board = new Board(renderer);
    player = new Player(renderer, mainFont, smallFont);
    
    // Define UI button rectangles in the new side panel
    submitButtonRect = { UI_PANEL_X, 40, 220, 50 };
    recallButtonRect = { UI_PANEL_X, 110, 220, 50 };
    resetButtonRect = { UI_PANEL_X, 180, 220, 50 }; 
    startOverButtonRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50 };

    isRunning = true;
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

        if (currentState == PLAYING) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (mouseX >= submitButtonRect.x && mouseX < submitButtonRect.x + submitButtonRect.w &&
                    mouseY >= submitButtonRect.y && mouseY < submitButtonRect.y + submitButtonRect.h) {
                    submitWord(); continue;
                }
                if (mouseX >= recallButtonRect.x && mouseX < recallButtonRect.x + recallButtonRect.w &&
                    mouseY >= recallButtonRect.y && mouseY < recallButtonRect.y + recallButtonRect.h) {
                    recallAllTiles(); continue;
                }
                if (mouseX >= resetButtonRect.x && mouseX < resetButtonRect.x + resetButtonRect.w &&
                    mouseY >= resetButtonRect.y && mouseY < resetButtonRect.y + resetButtonRect.h) {
                    resetLetters(); continue;
                }
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
        } else if (currentState == GAME_OVER) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (mouseX >= startOverButtonRect.x && mouseX < startOverButtonRect.x + startOverButtonRect.w &&
                    mouseY >= startOverButtonRect.y && mouseY < startOverButtonRect.y + startOverButtonRect.h) {
                    startOver();
                }
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
        recallAllTiles(); 
        player->resetRack();
    } else {
        std::cout << "No resets left! Game Over." << std::endl;
        currentState = GAME_OVER;
        if (player->getScore() > highScore) {
            highScore = player->getScore();
            saveHighScore();
        }
    }
}

void Game::startOver() {
    std::cout << "Starting a new game..." << std::endl;
    delete board;
    delete player;
    board = new Board(renderer);
    player = new Player(renderer, mainFont, smallFont);
    currentState = PLAYING;
}

void Game::loadHighScore() {
    std::ifstream file(HIGHSCORE_PATH);
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream file(HIGHSCORE_PATH);
    if (file.is_open()) {
        file << highScore;
        file.close();
    } else {
        std::cerr << "ERROR: Could not save high score to file." << std::endl;
    }
}

void Game::update() {}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, COLOR_BOARD_BG.r, COLOR_BOARD_BG.g, COLOR_BOARD_BG.b, COLOR_BOARD_BG.a);
    SDL_RenderClear(renderer);
    board->render();
    player->renderRack();
    renderUI();
    if (selectedTile && currentState == PLAYING) {
        selectedTile->render(mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2);
    }
    if (currentState == GAME_OVER) {
        renderGameOver();
    }
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    // --- Draw Button Backgrounds ---
    SDL_SetRenderDrawColor(renderer, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, COLOR_BUTTON.a);
    SDL_RenderFillRect(renderer, &submitButtonRect);
    SDL_RenderFillRect(renderer, &recallButtonRect);
    SDL_RenderFillRect(renderer, &resetButtonRect);

    // --- Draw Button Text ---
    SDL_Texture* submitTexture = TextureManager::LoadText(renderer, uiFont, "Submit Word", COLOR_BUTTON_TEXT);
    if (submitTexture) {
        int w, h; SDL_QueryTexture(submitTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { submitButtonRect.x + (submitButtonRect.w - w) / 2, submitButtonRect.y + (submitButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, submitTexture, NULL, &dest);
        SDL_DestroyTexture(submitTexture);
    }
    SDL_Texture* recallTexture = TextureManager::LoadText(renderer, uiFont, "Recall Tiles", COLOR_BUTTON_TEXT);
    if (recallTexture) {
        int w, h; SDL_QueryTexture(recallTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { recallButtonRect.x + (recallButtonRect.w - w) / 2, recallButtonRect.y + (recallButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, recallTexture, NULL, &dest);
        SDL_DestroyTexture(recallTexture);
    }
    SDL_Texture* resetTexture = TextureManager::LoadText(renderer, uiFont, "Reset Letters", COLOR_BUTTON_TEXT);
    if (resetTexture) {
        int w, h; SDL_QueryTexture(resetTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { resetButtonRect.x + (resetButtonRect.w - w) / 2, resetButtonRect.y + (resetButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, resetTexture, NULL, &dest);
        SDL_DestroyTexture(resetTexture);
    }

    // --- Draw Score Info Text ---
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    std::string scoreText = "Current Score: " + std::to_string(player->getScore());
    std::string livesText = "Resets Left: " + std::to_string(player->getLives());

    SDL_Texture* highScoreTexture = TextureManager::LoadText(renderer, uiFont, highScoreText, COLOR_BUTTON_TEXT);
    if(highScoreTexture) {
        int w, h; SDL_QueryTexture(highScoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 300, w, h};
        SDL_RenderCopy(renderer, highScoreTexture, NULL, &dest);
        SDL_DestroyTexture(highScoreTexture);
    }
    SDL_Texture* scoreTexture = TextureManager::LoadText(renderer, uiFont, scoreText, COLOR_BUTTON_TEXT);
    if(scoreTexture) {
        int w, h; SDL_QueryTexture(scoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 340, w, h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
        SDL_DestroyTexture(scoreTexture);
    }
    SDL_Texture* livesTexture = TextureManager::LoadText(renderer, uiFont, livesText, COLOR_BUTTON_TEXT);
    if(livesTexture) {
        int w, h; SDL_QueryTexture(livesTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 380, w, h};
        SDL_RenderCopy(renderer, livesTexture, NULL, &dest);
        SDL_DestroyTexture(livesTexture);
    }
}

void Game::renderGameOver() {
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_BG.r, COLOR_GAMEOVER_BG.g, COLOR_GAMEOVER_BG.b, COLOR_GAMEOVER_BG.a);
    SDL_RenderFillRect(renderer, &overlay);
    SDL_Rect dialog = {SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 150, 400, 300};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_DIALOG.r, COLOR_GAMEOVER_DIALOG.g, COLOR_GAMEOVER_DIALOG.b, COLOR_GAMEOVER_DIALOG.a);
    SDL_RenderFillRect(renderer, &dialog);
    SDL_Texture* goTexture = TextureManager::LoadText(renderer, gameOverFont, "Game Over", COLOR_BUTTON_TEXT);
    if (goTexture) {
        int w, h; SDL_QueryTexture(goTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH / 2 - w / 2, dialog.y + 30, w, h };
        SDL_RenderCopy(renderer, goTexture, NULL, &dest);
        SDL_DestroyTexture(goTexture);
    }
    std::string finalScoreText = "Final Score: " + std::to_string(player->getScore());
    SDL_Texture* scoreTexture = TextureManager::LoadText(renderer, uiFont, finalScoreText, COLOR_BUTTON_TEXT);
    if (scoreTexture) {
        int w, h; SDL_QueryTexture(scoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH / 2 - w / 2, dialog.y + 120, w, h };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
        SDL_DestroyTexture(scoreTexture);
    }
    SDL_SetRenderDrawColor(renderer, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, COLOR_BUTTON.a);
    SDL_RenderFillRect(renderer, &startOverButtonRect);
    SDL_Texture* buttonTexture = TextureManager::LoadText(renderer, uiFont, "Start Over", COLOR_BUTTON_TEXT);
    if (buttonTexture) {
        int w, h; SDL_QueryTexture(buttonTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { startOverButtonRect.x + (startOverButtonRect.w - w) / 2, startOverButtonRect.y + (startOverButtonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, buttonTexture, NULL, &dest);
        SDL_DestroyTexture(buttonTexture);
    }
}

void Game::cleanup() {
    delete board;
    delete player;
    delete dictionary;
    if (mainFont) TTF_CloseFont(mainFont);
    if (smallFont) TTF_CloseFont(smallFont);
    if (uiFont) TTF_CloseFont(uiFont);
    if (gameOverFont) TTF_CloseFont(gameOverFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}