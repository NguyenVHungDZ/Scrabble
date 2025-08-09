// -- src/Game.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h> 
#include <SDL_ttf.h>
#include "Game.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Board.h"
#include "Player.h"
#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <vector>

Game::Game() 
    : isRunning(false), currentState(PLAYING), highScore(0), window(nullptr), renderer(nullptr), 
      mainFont(nullptr), smallFont(nullptr), uiFont(nullptr), gameOverFont(nullptr), manualIconTexture(nullptr),
      board(nullptr), player(nullptr), dictionary(nullptr),
      selectedTile(nullptr), mouseX(0), mouseY(0) {}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) { return false; }
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "ERROR: SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }

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
    
    manualIconTexture = TextureManager::LoadImage(renderer, MANUAL_ICON_PATH);
    if (!manualIconTexture) {
        std::cerr << "ERROR: Failed to load manual icon!" << std::endl;
        return false;
    }

    loadHighScore();
    dictionary = new Dictionary(DICTIONARY_PATH);
    board = new Board(renderer);
    player = new Player(renderer, mainFont, smallFont);
    
    submitButtonRect = { UI_PANEL_X, 40, 220, 50 };
    recallButtonRect = { UI_PANEL_X, 110, 220, 50 };
    resetButtonRect = { UI_PANEL_X, 180, 220, 50 }; 
    startOverButtonRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50 };
    manualButtonRect = { SCREEN_WIDTH - (MANUAL_ICON_SIZE + 20), SCREEN_HEIGHT - (MANUAL_ICON_SIZE + 20), MANUAL_ICON_SIZE, MANUAL_ICON_SIZE };

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

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (currentState == MANUAL) {
                currentState = PLAYING; 
                continue; 
            }
            if (mouseX >= manualButtonRect.x && mouseX < manualButtonRect.x + manualButtonRect.w &&
                mouseY >= manualButtonRect.y && mouseY < manualButtonRect.y + manualButtonRect.h) {
                currentState = MANUAL; 
                continue; 
            }
            
            if (currentState == PLAYING) {
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
            } else if (currentState == GAME_OVER) {
                if (mouseX >= startOverButtonRect.x && mouseX < startOverButtonRect.x + startOverButtonRect.w &&
                    mouseY >= startOverButtonRect.y && mouseY < startOverButtonRect.y + startOverButtonRect.h) {
                    startOver();
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
    // 1. Find all words formed by the temporary tiles
    vector<WordPlacement> placements = board->findAllNewWords();

    if (placements.empty()) {
        cout << "Invalid placement. Words must be connected and form a single line." << endl;
        recallAllTiles();
        return;
    }

    // 2. Validate every new word against the dictionary
    bool allWordsValid = true;
    for (const auto& placement : placements) {
        if (!dictionary->isValidWord(placement.word)) {
            cout << "Word '" << placement.word << "' is not in the dictionary." << endl;
            allWordsValid = false;
            break; // One invalid word makes the whole move invalid
        }
    }

    // 3. If all words are valid, calculate score and finalize the turn
    if (allWordsValid) {
        int totalTurnScore = 0;
        int wordMultipliers = 1;

        // First, sum the base letter scores and find word multipliers
        for (const auto& placement : placements) {
            for (Tile* tile : placement.tiles) {
                 // Check if this tile is one of the newly placed ones
                bool isNewTile = false;
                for (Tile* tempTile : board->getTempPlacedTiles()) { // You'll need to add a simple getter for this
                    if (tile == tempTile) {
                        isNewTile = true;
                        break;
                    }
                }

                // Word bonuses are only triggered by newly placed tiles
                if(isNewTile) {
                    Bonus bonus = board->getBonusAt(tile->boardRow, tile->boardCol); // You'll need to add this getter too
                    if (bonus == DOUBLE_WORD || bonus == CENTER) wordMultipliers *= 2;
                    if (bonus == TRIPLE_WORD) wordMultipliers *= 3;
                }
            }
        }
        
        // Then, calculate score for each word and apply multipliers
        for (const auto& placement : placements) {
            int wordScore = board->calculateScore(placement);
            cout << "Word '" << placement.word << "' is valid! Score: " << wordScore << endl;
            totalTurnScore += wordScore;
        }

        totalTurnScore *= wordMultipliers;

        cout << "Total score for turn: " << totalTurnScore << endl;
        player->addScore(totalTurnScore);
        board->finalizeTurn();
        player->refillRack();

    } else {
        cout << "Your move was invalid. Recalling tiles." << endl;
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
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);
    board->render();
    player->renderRack(mouseX, mouseY);
    renderUI();
    if (selectedTile && currentState == PLAYING) {
        selectedTile->render(mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2, true, mouseX, mouseY);
    }
    if (currentState == GAME_OVER) {
        renderGameOver();
    }
    if (currentState == MANUAL) {
        renderManual();
    }
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    SDL_Rect uiPanelRect = { UI_PANEL_X - 20, 0, SCREEN_WIDTH - (UI_PANEL_X - 20), SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, COLOR_UI_PANEL.r, COLOR_UI_PANEL.g, COLOR_UI_PANEL.b, COLOR_UI_PANEL.a);
    SDL_RenderFillRect(renderer, &uiPanelRect);
    auto drawButton = [&](const SDL_Rect& rect, const std::string& text) {
        Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
        bool isHovered = (mouseX >= rect.x && mouseX < rect.x + rect.w && mouseY >= rect.y && mouseY < rect.y + rect.h);
        bool isClicked = isHovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
        const Color& bgColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
        SDL_Rect buttonRect = rect;
        SDL_Rect shadowRect = { rect.x + 3, rect.y + 3, rect.w, rect.h };
        if (isClicked) {
            buttonRect = shadowRect;
        } else {
            SDL_SetRenderDrawColor(renderer, COLOR_BUTTON_SHADOW.r, COLOR_BUTTON_SHADOW.g, COLOR_BUTTON_SHADOW.b, COLOR_BUTTON_SHADOW.a);
            SDL_RenderFillRect(renderer, &shadowRect);
        }
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &buttonRect);
        SDL_Texture* textTexture = TextureManager::LoadText(renderer, uiFont, text, COLOR_TEXT_LIGHT);
        if (textTexture) {
            int w, h; SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);
            SDL_Rect dest = { buttonRect.x + (buttonRect.w - w) / 2, buttonRect.y + (buttonRect.h - h) / 2, w, h };
            SDL_RenderCopy(renderer, textTexture, NULL, &dest);
            SDL_DestroyTexture(textTexture);
        }
    };
    drawButton(submitButtonRect, "Submit Word");
    drawButton(recallButtonRect, "Recall Tiles");
    drawButton(resetButtonRect, "Reset Letters");
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    std::string scoreText = "Current Score: " + std::to_string(player->getScore());
    std::string livesText = "Resets Left: " + std::to_string(player->getLives());
    SDL_Texture* highScoreTexture = TextureManager::LoadText(renderer, uiFont, highScoreText, COLOR_TEXT_LIGHT);
    if(highScoreTexture) {
        int w, h; SDL_QueryTexture(highScoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 300, w, h};
        SDL_RenderCopy(renderer, highScoreTexture, NULL, &dest);
        SDL_DestroyTexture(highScoreTexture);
    }
    SDL_Texture* scoreTexture = TextureManager::LoadText(renderer, uiFont, scoreText, COLOR_TEXT_LIGHT);
    if(scoreTexture) {
        int w, h; SDL_QueryTexture(scoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 340, w, h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
        SDL_DestroyTexture(scoreTexture);
    }
    SDL_Texture* livesTexture = TextureManager::LoadText(renderer, uiFont, livesText, COLOR_TEXT_LIGHT);
    if(livesTexture) {
        int w, h; SDL_QueryTexture(livesTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { UI_PANEL_X, 380, w, h};
        SDL_RenderCopy(renderer, livesTexture, NULL, &dest);
        SDL_DestroyTexture(livesTexture);
    }
    if (manualIconTexture) {
        bool isHovered = (mouseX >= manualButtonRect.x && mouseX < manualButtonRect.x + manualButtonRect.w &&
                          mouseY >= manualButtonRect.y && mouseY < manualButtonRect.y + manualButtonRect.h);
        if (isHovered) {
            SDL_SetTextureColorMod(manualIconTexture, 200, 200, 200); 
        } else {
            SDL_SetTextureColorMod(manualIconTexture, 255, 255, 255); 
        }
        SDL_RenderCopy(renderer, manualIconTexture, NULL, &manualButtonRect);
    }
}

void Game::renderGameOver() {
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_BG.r, COLOR_GAMEOVER_BG.g, COLOR_GAMEOVER_BG.b, COLOR_GAMEOVER_BG.a);
    SDL_RenderFillRect(renderer, &overlay);
    SDL_Rect dialog = {SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 150, 400, 300};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_DIALOG.r, COLOR_GAMEOVER_DIALOG.g, COLOR_GAMEOVER_DIALOG.b, COLOR_GAMEOVER_DIALOG.a);
    SDL_RenderFillRect(renderer, &dialog);
    SDL_Texture* goTexture = TextureManager::LoadText(renderer, gameOverFont, "Game Over", COLOR_TEXT_LIGHT);
    if (goTexture) {
        int w, h; SDL_QueryTexture(goTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH / 2 - w / 2, dialog.y + 30, w, h };
        SDL_RenderCopy(renderer, goTexture, NULL, &dest);
        SDL_DestroyTexture(goTexture);
    }
    std::string finalScoreText = "Final Score: " + std::to_string(player->getScore());
    SDL_Texture* scoreTexture = TextureManager::LoadText(renderer, uiFont, finalScoreText, COLOR_TEXT_LIGHT);
    if (scoreTexture) {
        int w, h; SDL_QueryTexture(scoreTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { SCREEN_WIDTH / 2 - w / 2, dialog.y + 120, w, h };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
        SDL_DestroyTexture(scoreTexture);
    }
    Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
    bool isHovered = (mouseX >= startOverButtonRect.x && mouseX < startOverButtonRect.x + startOverButtonRect.w && mouseY >= startOverButtonRect.y && mouseY < startOverButtonRect.y + startOverButtonRect.h);
    bool isClicked = isHovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
    const Color& bgColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    SDL_Rect buttonRect = startOverButtonRect;
    SDL_Rect shadowRect = { startOverButtonRect.x + 3, startOverButtonRect.y + 3, startOverButtonRect.w, startOverButtonRect.h };
    if (isClicked) {
        buttonRect = shadowRect;
    } else {
        SDL_SetRenderDrawColor(renderer, COLOR_BUTTON_SHADOW.r, COLOR_BUTTON_SHADOW.g, COLOR_BUTTON_SHADOW.b, COLOR_BUTTON_SHADOW.a);
        SDL_RenderFillRect(renderer, &shadowRect);
    }
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &buttonRect);
    SDL_Texture* buttonTexture = TextureManager::LoadText(renderer, uiFont, "Start Over", COLOR_TEXT_LIGHT);
    if (buttonTexture) {
        int w, h; SDL_QueryTexture(buttonTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = { buttonRect.x + (buttonRect.w - w) / 2, buttonRect.y + (buttonRect.h - h) / 2, w, h };
        SDL_RenderCopy(renderer, buttonTexture, NULL, &dest);
        SDL_DestroyTexture(buttonTexture);
    }
}

void Game::renderManual() {
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_BG.r, COLOR_GAMEOVER_BG.g, COLOR_GAMEOVER_BG.b, 240);
    SDL_RenderFillRect(renderer, &overlay);
    
    std::vector<std::string> manualLines = {
        "How to Play",
        "",
        "1. Drag letters from your rack to the board to form words.",
        "2. Words must be in a single row or column.",
        "3. Click 'Submit Word' to score points.",
        "4. Use 'Reset Letters' to get new tiles (costs 1 reset).",
        "5. The game ends when you have 0 resets left.",
        "",
        "Click anywhere to close this guide."
    };

    int startY = 150;
    for (const auto& line : manualLines) {
        SDL_Texture* tex = TextureManager::LoadText(renderer, uiFont, line, COLOR_TEXT_LIGHT);
        if (tex) {
            int w, h; SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            SDL_Rect dest = { SCREEN_WIDTH / 2 - w / 2, startY, w, h };
            SDL_RenderCopy(renderer, tex, NULL, &dest);
            SDL_DestroyTexture(tex);
            startY += h + 10;
        }
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
    if (manualIconTexture) SDL_DestroyTexture(manualIconTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit(); 
    TTF_Quit();
    SDL_Quit();
}