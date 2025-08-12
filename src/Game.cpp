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
#include <random>
#include <algorithm>

using namespace std;

Game::Game()
    : isRunning(false), currentState(MAIN_MENU), gameMode(ONE_PLAYER), highScore(0), window(nullptr), renderer(nullptr),
      mainFont(nullptr), smallFont(nullptr), uiFont(nullptr), gameOverFont(nullptr), manualIconTexture(nullptr), backButtonTexture(nullptr),
      board(nullptr), dictionary(nullptr), selectedTile(nullptr), mouseX(0), mouseY(0),
      isFirstTurn(true), consecutiveScorelessTurns(0),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr), currentPlayerIndex(0),
      player1NameInput("Player 1"), player2NameInput("Player 2"), activeInputFieldIndex(0),
      player1NameClicked(false), player2NameClicked(false) {}

Game::~Game()
{
    cleanup();
}

bool Game::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 || TTF_Init() == -1) return false;

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "ERROR: SDL_image could not initialize! IMG_Error: " << IMG_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) return false;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    mainFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE);
    smallFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_SMALL);
    uiFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_UI);
    gameOverFont = TTF_OpenFont(FONT_PATH.c_str(), FONT_SIZE_GAMEOVER);
    if (!mainFont || !smallFont || !uiFont || !gameOverFont) {
        cerr << "ERROR: Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }

    manualIconTexture = TextureManager::LoadImage(renderer, MANUAL_ICON_PATH);
    backButtonTexture = TextureManager::LoadImage(renderer, "assets/back_button.png");
    dictionary = new Dictionary(DICTIONARY_PATH);
    loadHighScore();

    // --- UI Button Definitions ---
    // Main Menu
    onePlayerButtonRect = {SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 60, 250, 50};
    twoPlayerButtonRect = {SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 + 10, 250, 50};
    // Name Entry
    player1InputRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 80, 300, 50};
    player2InputRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 300, 50};
    startGameButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 80, 200, 50};
    backButtonRect = {20, 20, 50, 50};
    // In-Game
    inGameBackButtonRect = {UI_PANEL_X, SCREEN_HEIGHT - 70, 50, 50};
    submitButtonRect = {UI_PANEL_X, 40, 220, 50};
    recallButtonRect = {UI_PANEL_X, 100, 220, 50};
    resetButtonRect = {UI_PANEL_X, 160, 220, 50};
    passButtonRect = {UI_PANEL_X, 220, 220, 50};
    // Game Over
    startOverButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50};
    manualButtonRect = {SCREEN_WIDTH - (MANUAL_ICON_SIZE + 20), SCREEN_HEIGHT - (MANUAL_ICON_SIZE + 20), MANUAL_ICON_SIZE, MANUAL_ICON_SIZE};

    isRunning = true;
    return true;
}

void Game::startGame(GameMode mode, string p1Name, string p2Name) {
    delete board;
    delete player1;
    delete player2;

    gameMode = mode;
    int boardYOffset = (gameMode == ONE_PLAYER) ? BOARD_Y_OFFSET_1P : BOARD_Y_OFFSET_2P;
    board = new Board(renderer, uiFont, boardYOffset);
    initializeTileBag();

    player1 = new Player(p1Name, renderer, mainFont, smallFont);
    player1->refillRack(tileBag, tileValues);
    
    if (gameMode == TWO_PLAYER) {
        player2 = new Player(p2Name, renderer, mainFont, smallFont);
        player2->refillRack(tileBag, tileValues);
    } else {
        player2 = nullptr;
    }

    currentPlayer = player1;
    currentPlayerIndex = 0;
    isFirstTurn = true;
    consecutiveScorelessTurns = 0;
    selectedTile = nullptr;
    currentState = PLAYING;
}

void Game::run()
{
    while (isRunning)
    {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT) isRunning = false;
        
        if (currentState == NAME_ENTRY) {
            if (e.type == SDL_TEXTINPUT) {
                if (activeInputFieldIndex == 0) player1NameInput += e.text.text;
                else player2NameInput += e.text.text;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    if (activeInputFieldIndex == 0 && !player1NameInput.empty()) {
                        player1NameInput.pop_back();
                    } else if (activeInputFieldIndex == 1 && !player2NameInput.empty()) {
                        player2NameInput.pop_back();
                    }
                }
            }
        }

        SDL_GetMouseState(&mouseX, &mouseY);

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (currentState == MAIN_MENU) {
                if (mouseX >= onePlayerButtonRect.x && mouseX < onePlayerButtonRect.x + onePlayerButtonRect.w &&
                    mouseY >= onePlayerButtonRect.y && mouseY < onePlayerButtonRect.y + onePlayerButtonRect.h) {
                    startGame(ONE_PLAYER);
                }
                if (mouseX >= twoPlayerButtonRect.x && mouseX < twoPlayerButtonRect.x + twoPlayerButtonRect.w &&
                    mouseY >= twoPlayerButtonRect.y && mouseY < twoPlayerButtonRect.y + twoPlayerButtonRect.h) {
                    currentState = NAME_ENTRY;
                    SDL_StartTextInput();
                }
            } else if (currentState == NAME_ENTRY) {
                if (mouseX >= backButtonRect.x && mouseX < backButtonRect.x + backButtonRect.w &&
                    mouseY >= backButtonRect.y && mouseY < backButtonRect.y + backButtonRect.h) {
                    SDL_StopTextInput();
                    startOver();
                } else if (mouseX >= player1InputRect.x && mouseX < player1InputRect.x + player1InputRect.w &&
                    mouseY >= player1InputRect.y && mouseY < player1InputRect.y + player1InputRect.h) {
                    activeInputFieldIndex = 0;
                    if (!player1NameClicked) {
                        player1NameInput.clear();
                        player1NameClicked = true;
                    }
                } else if (mouseX >= player2InputRect.x && mouseX < player2InputRect.x + player2InputRect.w &&
                           mouseY >= player2InputRect.y && mouseY < player2InputRect.y + player2InputRect.h) {
                    activeInputFieldIndex = 1;
                    if (!player2NameClicked) {
                        player2NameInput.clear();
                        player2NameClicked = true;
                    }
                } else if (mouseX >= startGameButtonRect.x && mouseX < startGameButtonRect.x + startGameButtonRect.w &&
                           mouseY >= startGameButtonRect.y && mouseY < startGameButtonRect.y + startGameButtonRect.h) {
                    SDL_StopTextInput();
                    startGame(TWO_PLAYER, player1NameInput.empty() ? "Player 1" : player1NameInput, player2NameInput.empty() ? "Player 2" : player2NameInput);
                }
            } else if (currentState == PLAYING) {
                if (mouseX >= inGameBackButtonRect.x && mouseX < inGameBackButtonRect.x + inGameBackButtonRect.w &&
                    mouseY >= inGameBackButtonRect.y && mouseY < inGameBackButtonRect.y + inGameBackButtonRect.h) {
                    startOver();
                } else if (mouseX >= submitButtonRect.x && mouseX < submitButtonRect.x + submitButtonRect.w &&
                    mouseY >= submitButtonRect.y && mouseY < submitButtonRect.y + submitButtonRect.h) {
                    submitWord();
                } else if (mouseX >= recallButtonRect.x && mouseX < recallButtonRect.x + recallButtonRect.w &&
                    mouseY >= recallButtonRect.y && mouseY < recallButtonRect.y + recallButtonRect.h) {
                    recallAllTiles();
                } else if (gameMode == ONE_PLAYER && mouseX >= resetButtonRect.x && mouseX < resetButtonRect.x + resetButtonRect.w &&
                    mouseY >= resetButtonRect.y && mouseY < resetButtonRect.y + resetButtonRect.h) {
                    resetLetters();
                } else if (gameMode == TWO_PLAYER && mouseX >= passButtonRect.x && mouseX < passButtonRect.x + passButtonRect.w &&
                    mouseY >= passButtonRect.y && mouseY < passButtonRect.y + passButtonRect.h) {
                    passTurn();
                } else if (mouseX >= manualButtonRect.x && mouseX < manualButtonRect.x + manualButtonRect.w &&
                    mouseY >= manualButtonRect.y && mouseY < manualButtonRect.y + manualButtonRect.h) {
                    currentState = MANUAL;
                } else {
                    // --- Tile Pickup Logic ---
                    int rackStartX = RACK_AREA_X + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
                    int rackY = (gameMode == TWO_PLAYER && currentPlayer == player2) ? RACK_Y_POS_P2 : RACK_Y_POS_P1;
                    rackY += (RACK_HEIGHT - TILE_SIZE) / 2;

                    for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
                        SDL_Rect tileRect = {rackStartX + i * (TILE_SIZE + 5), rackY, TILE_SIZE, TILE_SIZE};
                        if (currentPlayer->getTileFromRack(i) && mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                            selectedTile = currentPlayer->getTileFromRack(i);
                            currentPlayer->removeTileFromRack(i);
                            break;
                        }
                    }
                }
            } else if (currentState == GAME_OVER) {
                if (mouseX >= startOverButtonRect.x && mouseX < startOverButtonRect.x + startOverButtonRect.w &&
                    mouseY >= startOverButtonRect.y && mouseY < startOverButtonRect.y + startOverButtonRect.h) {
                    startOver();
                }
            } else if (currentState == MANUAL) {
                currentState = PLAYING;
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP) {
            if (selectedTile) {
                int boardY = (gameMode == ONE_PLAYER) ? BOARD_Y_OFFSET_1P : BOARD_Y_OFFSET_2P;
                int boardCol = (mouseX - BOARD_X_OFFSET) / TILE_SIZE;
                int boardRow = (mouseY - boardY) / TILE_SIZE;

                if (boardRow >= 0 && boardRow < BOARD_DIMENSION && boardCol >= 0 && boardCol < BOARD_DIMENSION && !board->isOccupied(boardRow, boardCol)) {
                    board->placeTemporaryTile(selectedTile, boardRow, boardCol);
                } else {
                    currentPlayer->returnTileToRack(selectedTile);
                }
                selectedTile = nullptr;
            }
        }
    }
}

void Game::submitWord()
{
    if (board->getTempPlacedTiles().empty()) {
        cout << "You must place at least one tile." << endl;
        return;
    }

    if (isFirstTurn) {
        bool hasCenterTile = false;
        for (Tile* tile : board->getTempPlacedTiles()) {
            if (tile->boardRow == BOARD_DIMENSION / 2 && tile->boardCol == BOARD_DIMENSION / 2) {
                hasCenterTile = true;
                break;
            }
        }
        if (!hasCenterTile) {
            cout << "First word must be placed on the center star square." << endl;
            recallAllTiles();
            return;
        }
    }

    vector<WordPlacement> placements = board->findAllNewWords();
    if (placements.empty()) {
        cout << "Invalid placement. Words must be connected and form a single line." << endl;
        recallAllTiles();
        consecutiveScorelessTurns++;
        switchPlayer();
        checkEndGameConditions();
        return;
    }

    bool allWordsValid = true;
    for (const auto& placement : placements) {
        if (!dictionary->isValidWord(placement.word)) {
            cout << "Word '" << placement.word << "' is not in the dictionary." << endl;
            allWordsValid = false;
            break;
        }
    }

    if (allWordsValid) {
        int totalTurnScore = 0;
        int wordMultipliers = 1;

        for (const auto& placement : placements) {
            for (Tile* tile : placement.tiles) {
                bool isNewTile = false;
                for (Tile* tempTile : board->getTempPlacedTiles()) {
                    if (tile == tempTile) {
                        isNewTile = true;
                        break;
                    }
                }
                if (isNewTile) {
                    Bonus bonus = board->getBonusAt(tile->boardRow, tile->boardCol);
                    if (bonus == DOUBLE_WORD || bonus == CENTER) wordMultipliers *= 2;
                    if (bonus == TRIPLE_WORD) wordMultipliers *= 3;
                }
            }
        }

        for (const auto& placement : placements) {
            int wordScore = board->calculateScore(placement);
            cout << "Word '" << placement.word << "' is valid! Score: " << wordScore << endl;
            totalTurnScore += wordScore;
        }

        totalTurnScore *= wordMultipliers;

        if (totalTurnScore > 0) {
            consecutiveScorelessTurns = 0;
        } else {
            consecutiveScorelessTurns++;
        }

        cout << "Total score for turn: " << totalTurnScore << endl;
        currentPlayer->addScore(totalTurnScore);
        board->finalizeTurn();
        currentPlayer->refillRack(tileBag, tileValues);
        isFirstTurn = false;

        checkEndGameConditions();
        if(currentState == PLAYING) switchPlayer();

    } else {
        cout << "Your move was invalid. Recalling tiles." << endl;
        recallAllTiles();
        consecutiveScorelessTurns++;
        checkEndGameConditions();
        if(currentState == PLAYING) switchPlayer();
    }
}

void Game::passTurn() {
    cout << currentPlayer->getName() << " passed the turn." << endl;
    recallAllTiles();
    consecutiveScorelessTurns++;
    checkEndGameConditions();
    if(currentState == PLAYING) switchPlayer();
}

void Game::switchPlayer() {
    if (gameMode == TWO_PLAYER) {
        currentPlayerIndex = (currentPlayerIndex + 1) % 2;
        currentPlayer = (currentPlayerIndex == 0) ? player1 : player2;
        cout << "It is now " << currentPlayer->getName() << "'s turn." << endl;
    }
}

void Game::checkEndGameConditions() {
    if (!currentPlayer->hasTiles() && tileBag.empty()) {
        cout << "Game Over! " << currentPlayer->getName() << " used all their tiles." << endl;
        Player* otherPlayer = (gameMode == TWO_PLAYER && currentPlayer == player1) ? player2 : player1;
        if (otherPlayer) {
            int penalty = 0;
            for(Tile* tile : otherPlayer->getRack()){
                if(tile) penalty += tile->getValue();
            }
            otherPlayer->addScore(-penalty);
            currentPlayer->addScore(penalty);
        }
        currentState = GAME_OVER;
    }

    if (consecutiveScorelessTurns >= 6) {
        cout << "Game Over! 6 consecutive scoreless turns." << endl;
        currentState = GAME_OVER;
    }

    if (currentState == GAME_OVER) {
        int finalScore = player1->getScore();
        if (player2) {
             finalScore = max(finalScore, player2->getScore());
        }
        if (finalScore > highScore) {
            highScore = finalScore;
            saveHighScore();
        }
    }
}

void Game::recallAllTiles()
{
    board->recallTiles(currentPlayer->getRack());
}

void Game::resetLetters()
{
    if (currentPlayer->getLives() > 0)
    {
        recallAllTiles();
        currentPlayer->resetRack(tileBag);
        currentPlayer->refillRack(tileBag, tileValues);
        currentPlayer->useLife();
    }
    else
    {
        cout << "No resets left! Game Over." << endl;
        currentState = GAME_OVER;
        checkEndGameConditions(); 
    }
}

void Game::startOver()
{
    cout << "Returning to main menu..." << endl;
    delete board; board = nullptr;
    delete player1; player1 = nullptr;
    delete player2; player2 = nullptr;
    currentState = MAIN_MENU;
    player1NameInput = "Player 1";
    player2NameInput = "Player 2";
    player1NameClicked = false;
    player2NameClicked = false;
}

void Game::update() {}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    switch (currentState) {
        case MAIN_MENU:
            renderMainMenu();
            break;
        case NAME_ENTRY:
            renderNameEntry();
            break;
        case PLAYING:
        case MANUAL:
            board->render();
            if (gameMode == TWO_PLAYER) {
                if(player1) player1->renderRack(mouseX, mouseY, RACK_Y_POS_P1, currentPlayer == player1);
                if(player2) player2->renderRack(mouseX, mouseY, RACK_Y_POS_P2, currentPlayer == player2);
            } else {
                if(player1) player1->renderRack(mouseX, mouseY, RACK_Y_POS_P1, true);
            }
            renderUI();
            if (selectedTile) {
                selectedTile->render(mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2, true, mouseX, mouseY);
            }
            if (currentState == MANUAL) renderManual();
            break;
        case GAME_OVER:
            board->render();
             if (gameMode == TWO_PLAYER) {
                if(player1) player1->renderRack(mouseX, mouseY, RACK_Y_POS_P1, false);
                if(player2) player2->renderRack(mouseX, mouseY, RACK_Y_POS_P2, false);
            } else {
                if(player1) player1->renderRack(mouseX, mouseY, RACK_Y_POS_P1, false);
            }
            renderUI();
            renderGameOver();
            break;
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMainMenu() {
    SDL_Texture* titleTexture = TextureManager::LoadText(renderer, gameOverFont, "Scrabble", COLOR_TEXT_DARK);
    if (titleTexture) {
        int w, h;
        SDL_QueryTexture(titleTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = {SCREEN_WIDTH / 2 - w / 2, SCREEN_HEIGHT / 2 - 150, w, h};
        SDL_RenderCopy(renderer, titleTexture, NULL, &dest);
        SDL_DestroyTexture(titleTexture);
    }
    
    auto drawButton = [&](const SDL_Rect& rect, const string& text) {
        bool isHovered = (mouseX >= rect.x && mouseX < rect.x + rect.w && mouseY >= rect.y && mouseY < rect.y + rect.h);
        const Color& bgColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
        SDL_Rect shadowRect = {rect.x + 3, rect.y + 3, rect.w, rect.h};
        SDL_SetRenderDrawColor(renderer, COLOR_BUTTON_SHADOW.r, COLOR_BUTTON_SHADOW.g, COLOR_BUTTON_SHADOW.b, COLOR_BUTTON_SHADOW.a);
        SDL_RenderFillRect(renderer, &shadowRect);
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_Texture* textTexture = TextureManager::LoadText(renderer, uiFont, text, COLOR_TEXT_LIGHT);
        if (textTexture) {
            int w, h;
            SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);
            SDL_Rect dest = {rect.x + (rect.w - w) / 2, rect.y + (rect.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, textTexture, NULL, &dest);
            SDL_DestroyTexture(textTexture);
        }
    };

    drawButton(onePlayerButtonRect, "1 Player");
    drawButton(twoPlayerButtonRect, "2 Player");
}

void Game::renderNameEntry() {
    // Title
    SDL_Texture* titleTexture = TextureManager::LoadText(renderer, gameOverFont, "Enter Player Names", COLOR_TEXT_DARK);
    if (titleTexture) {
        int w, h; SDL_QueryTexture(titleTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = {SCREEN_WIDTH / 2 - w / 2, SCREEN_HEIGHT / 2 - 180, w, h};
        SDL_RenderCopy(renderer, titleTexture, NULL, &dest);
        SDL_DestroyTexture(titleTexture);
    }

    // Back Button
    if (backButtonTexture) {
        bool isHovered = (mouseX >= backButtonRect.x && mouseX < backButtonRect.x + backButtonRect.w &&
                          mouseY >= backButtonRect.y && mouseY < backButtonRect.y + backButtonRect.h);
        if (isHovered) {
            SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200);
        } else {
            SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255);
        }
        SDL_RenderCopy(renderer, backButtonTexture, NULL, &backButtonRect);
    }

    auto drawInputField = [&](const SDL_Rect& rect, const string& label, const string& text, bool isActive) {
        // Draw label
        SDL_Texture* labelTex = TextureManager::LoadText(renderer, uiFont, label, COLOR_TEXT_DARK);
        if(labelTex) {
            int w, h; SDL_QueryTexture(labelTex, NULL, NULL, &w, &h);
            SDL_Rect dest = {rect.x, rect.y - h - 5, w, h};
            SDL_RenderCopy(renderer, labelTex, NULL, &dest);
            SDL_DestroyTexture(labelTex);
        }

        // Draw input box background
        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Draw text
        string displayText = text;
        if (isActive) displayText += "_";
        SDL_Texture* textTex = TextureManager::LoadText(renderer, uiFont, displayText.empty() ? " " : displayText, COLOR_TEXT_DARK);
         if(textTex) {
            int w, h; SDL_QueryTexture(textTex, NULL, NULL, &w, &h);
            SDL_Rect dest = {rect.x + 10, rect.y + (rect.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, textTex, NULL, &dest);
            SDL_DestroyTexture(textTex);
        }

        // Draw border
        if (isActive) SDL_SetRenderDrawColor(renderer, COLOR_BUTTON_HOVER.r, COLOR_BUTTON_HOVER.g, COLOR_BUTTON_HOVER.b, 255);
        else SDL_SetRenderDrawColor(renderer, COLOR_UI_PANEL.r, COLOR_UI_PANEL.g, COLOR_UI_PANEL.b, 255);
        SDL_RenderDrawRect(renderer, &rect);
    };

    drawInputField(player1InputRect, "Player 1 Name:", player1NameInput, activeInputFieldIndex == 0);
    drawInputField(player2InputRect, "Player 2 Name:", player2NameInput, activeInputFieldIndex == 1);

    // Draw Start Game button
    bool isHovered = (mouseX >= startGameButtonRect.x && mouseX < startGameButtonRect.x + startGameButtonRect.w && mouseY >= startGameButtonRect.y && mouseY < startGameButtonRect.y + startGameButtonRect.h);
    const Color& bgColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &startGameButtonRect);
    SDL_Texture* buttonTexture = TextureManager::LoadText(renderer, uiFont, "Start Game", COLOR_TEXT_LIGHT);
    if (buttonTexture) {
        int w, h; SDL_QueryTexture(buttonTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = {startGameButtonRect.x + (startGameButtonRect.w - w) / 2, startGameButtonRect.y + (startGameButtonRect.h - h) / 2, w, h};
        SDL_RenderCopy(renderer, buttonTexture, NULL, &dest);
        SDL_DestroyTexture(buttonTexture);
    }
}

void Game::renderUI()
{
    SDL_Rect uiPanelRect = {UI_PANEL_X - 20, 0, SCREEN_WIDTH - (UI_PANEL_X - 20), SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_UI_PANEL.r, COLOR_UI_PANEL.g, COLOR_UI_PANEL.b, COLOR_UI_PANEL.a);
    SDL_RenderFillRect(renderer, &uiPanelRect);

    // Render in-game back button
    if (backButtonTexture) {
        bool isHovered = (mouseX >= inGameBackButtonRect.x && mouseX < inGameBackButtonRect.x + inGameBackButtonRect.w &&
                          mouseY >= inGameBackButtonRect.y && mouseY < inGameBackButtonRect.y + inGameBackButtonRect.h);
        if (isHovered) {
            SDL_SetTextureColorMod(backButtonTexture, 200, 200, 200);
        } else {
            SDL_SetTextureColorMod(backButtonTexture, 255, 255, 255);
        }
        SDL_RenderCopy(renderer, backButtonTexture, NULL, &inGameBackButtonRect);
    }

    auto drawButton = [&](const SDL_Rect& rect, const string& text, bool enabled = true) {
        Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
        bool isHovered = enabled && (mouseX >= rect.x && mouseX < rect.x + rect.w && mouseY >= rect.y && mouseY < rect.y + rect.h);
        bool isClicked = isHovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
        
        Color bgColor = enabled ? COLOR_BUTTON : Color{128, 128, 128, 255};
        if(enabled && isHovered) bgColor = COLOR_BUTTON_HOVER;

        SDL_Rect buttonRect = rect;
        SDL_Rect shadowRect = {rect.x + 3, rect.y + 3, rect.w, rect.h};
        if (isClicked) buttonRect = shadowRect;
        else {
            SDL_SetRenderDrawColor(renderer, COLOR_BUTTON_SHADOW.r, COLOR_BUTTON_SHADOW.g, COLOR_BUTTON_SHADOW.b, COLOR_BUTTON_SHADOW.a);
            SDL_RenderFillRect(renderer, &shadowRect);
        }
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &buttonRect);
        SDL_Texture* textTexture = TextureManager::LoadText(renderer, uiFont, text, COLOR_TEXT_LIGHT);
        if (textTexture) {
            int w, h; SDL_QueryTexture(textTexture, NULL, NULL, &w, &h);
            SDL_Rect dest = {buttonRect.x + (buttonRect.w - w) / 2, buttonRect.y + (buttonRect.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, textTexture, NULL, &dest);
            SDL_DestroyTexture(textTexture);
        }
    };
    
    bool canReset = currentPlayer && currentPlayer->getLives() > 0;
    drawButton(submitButtonRect, "Submit Word");
    drawButton(recallButtonRect, "Recall Tiles");
    if (gameMode == ONE_PLAYER) {
        drawButton(resetButtonRect, "Reset Letters", canReset);
    }
    if (gameMode == TWO_PLAYER) {
        drawButton(passButtonRect, "Pass Turn");
    }

    int textY = 300;
    auto renderText = [&](const string& text, int x, int& y, const Color& color) {
        SDL_Texture* texture = TextureManager::LoadText(renderer, uiFont, text, color);
        if (texture) {
            int w, h; SDL_QueryTexture(texture, NULL, NULL, &w, &h);
            SDL_Rect dest = {x, y, w, h};
            SDL_RenderCopy(renderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
            y += h + 10;
        }
    };

    if (gameMode == TWO_PLAYER) {
        string turnText = "Turn: " + currentPlayer->getName();
        renderText(turnText, UI_PANEL_X, textY, COLOR_TEXT_LIGHT);
        textY += 10;

        int p1_text_y = textY;
        int p2_text_y = textY + 35; 

        SDL_Rect p1_highlightRect = {UI_PANEL_X - 10, p1_text_y - 5, 240, 35 };
        SDL_Rect p2_highlightRect = {UI_PANEL_X - 10, p2_text_y - 5, 240, 35 };

        if (currentPlayer == player1) {
            for (int i = 0; i < p1_highlightRect.w; ++i) {
                float ratio = (float)i / (float)p1_highlightRect.w;
                Uint8 alpha = 80 * (1.0 - ratio);
                SDL_SetRenderDrawColor(renderer, COLOR_TILE_NORMAL.r, COLOR_TILE_NORMAL.g, COLOR_TILE_NORMAL.b, alpha);
                SDL_RenderDrawLine(renderer, p1_highlightRect.x + i, p1_highlightRect.y, p1_highlightRect.x + i, p1_highlightRect.y + p1_highlightRect.h);
            }
        } else { 
            for (int i = 0; i < p2_highlightRect.w; ++i) {
                float ratio = (float)i / (float)p2_highlightRect.w;
                Uint8 alpha = 80 * (1.0 - ratio);
                SDL_SetRenderDrawColor(renderer, COLOR_TILE_NORMAL.r, COLOR_TILE_NORMAL.g, COLOR_TILE_NORMAL.b, alpha);
                SDL_RenderDrawLine(renderer, p2_highlightRect.x + i, p2_highlightRect.y, p2_highlightRect.x + i, p2_highlightRect.y + p2_highlightRect.h);
            }
        }
        
        renderText(player1->getName() + ": " + to_string(player1->getScore()), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);
        renderText(player2->getName() + ": " + to_string(player2->getScore()), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);

    } else {
        if(player1) renderText("Score: " + to_string(player1->getScore()), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);
    }
    
    textY += 10;
    renderText("High Score: " + to_string(highScore), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);
    if(currentPlayer && gameMode == ONE_PLAYER) renderText("Resets Left: " + to_string(currentPlayer->getLives()), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);
    renderText("Tiles in Bag: " + to_string(tileBag.size()), UI_PANEL_X, textY, COLOR_TEXT_LIGHT);

    if (manualIconTexture) {
        SDL_RenderCopy(renderer, manualIconTexture, NULL, &manualButtonRect);
    }
}

void Game::renderGameOver()
{
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_BG.r, COLOR_GAMEOVER_BG.g, COLOR_GAMEOVER_BG.b, COLOR_GAMEOVER_BG.a);
    SDL_RenderFillRect(renderer, &overlay);
    SDL_Rect dialog = {SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 150, 500, 300};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_DIALOG.r, COLOR_GAMEOVER_DIALOG.g, COLOR_GAMEOVER_DIALOG.b, COLOR_GAMEOVER_DIALOG.a);
    SDL_RenderFillRect(renderer, &dialog);

    int textY = dialog.y + 30;
    auto renderText = [&](const string& text, TTF_Font* font, const Color& color) {
        SDL_Texture* texture = TextureManager::LoadText(renderer, font, text, color);
        if (texture) {
            int w, h; SDL_QueryTexture(texture, NULL, NULL, &w, &h);
            SDL_Rect dest = {SCREEN_WIDTH / 2 - w / 2, textY, w, h};
            SDL_RenderCopy(renderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
            textY += h + 15;
        }
    };
    
    renderText("Game Over", gameOverFont, COLOR_TEXT_LIGHT);
    textY += 10;

    if (gameMode == TWO_PLAYER) {
        renderText(player1->getName() + " Final Score: " + to_string(player1->getScore()), uiFont, COLOR_TEXT_LIGHT);
        renderText(player2->getName() + " Final Score: " + to_string(player2->getScore()), uiFont, COLOR_TEXT_LIGHT);
        string winnerText = "It's a Tie!";
        if (player1->getScore() > player2->getScore()) winnerText = player1->getName() + " Wins!";
        else if (player2->getScore() > player1->getScore()) winnerText = player2->getName() + " Wins!";
        renderText(winnerText, uiFont, COLOR_TILE_NORMAL);
    } else {
        renderText("Final Score: " + to_string(player1->getScore()), uiFont, COLOR_TEXT_LIGHT);
    }

    SDL_Rect buttonRect = {SCREEN_WIDTH / 2 - 100, dialog.y + dialog.h - 70, 200, 50};
    SDL_SetRenderDrawColor(renderer, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, COLOR_BUTTON.a);
    SDL_RenderFillRect(renderer, &buttonRect);
    SDL_Texture* buttonTexture = TextureManager::LoadText(renderer, uiFont, "Main Menu", COLOR_TEXT_LIGHT);
    if (buttonTexture) {
        int w, h; SDL_QueryTexture(buttonTexture, NULL, NULL, &w, &h);
        SDL_Rect dest = {buttonRect.x + (buttonRect.w - w) / 2, buttonRect.y + (buttonRect.h - h) / 2, w, h};
        SDL_RenderCopy(renderer, buttonTexture, NULL, &dest);
        SDL_DestroyTexture(buttonTexture);
    }
    startOverButtonRect = buttonRect;
}

void Game::cleanup()
{
    SDL_StopTextInput();
    delete board;
    delete player1;
    delete player2;
    delete dictionary;
    if (mainFont) TTF_CloseFont(mainFont);
    if (smallFont) TTF_CloseFont(smallFont);
    if (uiFont) TTF_CloseFont(uiFont);
    if (gameOverFont) TTF_CloseFont(gameOverFont);
    if (manualIconTexture) SDL_DestroyTexture(manualIconTexture);
    if (backButtonTexture) SDL_DestroyTexture(backButtonTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Game::loadHighScore() {
    ifstream file(HIGHSCORE_PATH);
    if (file.is_open()) { file >> highScore; file.close(); }
    else { highScore = 0; }
}

void Game::saveHighScore() {
    ofstream file(HIGHSCORE_PATH);
    if (file.is_open()) { file << highScore; file.close(); }
    else { cerr << "ERROR: Could not save high score to file." << endl; }
}

void Game::renderManual() {
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, COLOR_GAMEOVER_BG.r, COLOR_GAMEOVER_BG.g, COLOR_GAMEOVER_BG.b, 240);
    SDL_RenderFillRect(renderer, &overlay);
    vector<string> manualLines = {
        "How to Play", "",
        "1. Drag letters from your rack to the board to make a word.",
        "2. Words must be in a single row or column.",
        "3. The first word must cover the center star.",
        "4. Click 'Submit Word' to score points and end your turn.",
        "5. Click 'Pass Turn' to skip your turn.",
        "6. Use 'Reset Letters' to get new tiles (costs 1 reset).",
        "7. The game ends when a player runs out of tiles OR",
        "   after 6 consecutive scoreless turns.",
        "",
        "Click anywhere to close this guide."};
    int startY = 150;
    for (const auto &line : manualLines) {
        SDL_Texture *tex = TextureManager::LoadText(renderer, uiFont, line, COLOR_TEXT_LIGHT);
        if (tex) {
            int w, h; SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            SDL_Rect dest = {SCREEN_WIDTH / 2 - w / 2, startY, w, h};
            SDL_RenderCopy(renderer, tex, NULL, &dest);
            SDL_DestroyTexture(tex);
            startY += h + 10;
        }
    }
}

void Game::initializeTileBag() {
    tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}
    };
    map<char, int> tileCounts = {
        {'A', 9}, {'B', 2}, {'C', 2}, {'D', 4}, {'E', 12}, {'F', 2}, {'G', 3},
        {'H', 2}, {'I', 9}, {'J', 1}, {'K', 1}, {'L', 4}, {'M', 2}, {'N', 6},
        {'O', 8}, {'P', 2}, {'Q', 1}, {'R', 6}, {'S', 4}, {'T', 6}, {'U', 4},
        {'V', 2}, {'W', 2}, {'X', 1}, {'Y', 2}, {'Z', 1}
    };
    tileBag.clear();
    for (auto const& [letter, count] : tileCounts) {
        for (int i = 0; i < count; ++i) tileBag.push_back(letter);
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(tileBag.begin(), tileBag.end(), g);
}
