// -- src/Player.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Player.h"
#include "Constants.h"
#include <random>
#include <algorithm>
#include <iostream>

Player::Player(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont) 
    : renderer(renderer), font(font), smallFont(smallFont), score(0), lives(3) { 
    initializeTileBag();
    rack.resize(PLAYER_RACK_SIZE, nullptr);
    refillRack();
}

Player::~Player() {
    for (Tile* tile : rack) { delete tile; }
    rack.clear();
}

void Player::refillRack() {
    for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
        if (rack[i] == nullptr && !tileBag.empty()) {
            char letter = tileBag.back();
            tileBag.pop_back();
            rack[i] = new Tile(letter, tileValues[letter], renderer, font, smallFont);
            rack[i]->rackIndex = i;
        }
    }
}

void Player::resetRack() {
    if (lives <= 0) return; 
    lives--; 
    for (size_t i = 0; i < rack.size(); ++i) {
        if (rack[i] != nullptr) {
            if (rack[i]->getLetter() != ' ') {
                 tileBag.push_back(rack[i]->getLetter());
            }
            delete rack[i];
            rack[i] = nullptr;
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tileBag.begin(), tileBag.end(), g);
    refillRack();
}

void Player::renderRack(int mouseX, int mouseY) {
    SDL_Rect rackRect = { BOARD_X_OFFSET, RACK_Y_POS, BOARD_SIZE_PX, RACK_HEIGHT };
    SDL_SetRenderDrawColor(renderer, COLOR_RACK_BG.r, COLOR_RACK_BG.g, COLOR_RACK_BG.b, COLOR_RACK_BG.a);
    SDL_RenderFillRect(renderer, &rackRect);
    int startX = BOARD_X_OFFSET + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
    int rackTileY = RACK_Y_POS + (RACK_HEIGHT - TILE_SIZE) / 2;
    for (size_t i = 0; i < rack.size(); ++i) {
        if (rack[i]) {
            rack[i]->render(startX + i * (TILE_SIZE + 5), rackTileY, false, mouseX, mouseY);
        }
    }
}

Tile* Player::getTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) return rack[index];
    return nullptr;
}

void Player::removeTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) { rack[index] = nullptr; }
}

void Player::returnTileToRack(Tile* tile) {
    if (tile && tile->rackIndex >= 0 && (size_t)tile->rackIndex < rack.size()) {
        rack[tile->rackIndex] = tile;
    }
}

void Player::initializeTileBag() {
    tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}, {' ', 0}
    };
    std::map<char, int> tileCounts = {
        {'A', 9}, {'B', 2}, {'C', 2}, {'D', 4}, {'E', 12}, {'F', 2}, {'G', 3},
        {'H', 2}, {'I', 9}, {'J', 1}, {'K', 1}, {'L', 4}, {'M', 2}, {'N', 6},
        {'O', 8}, {'P', 2}, {'Q', 1}, {'R', 6}, {'S', 4}, {'T', 6}, {'U', 4},
        {'V', 2}, {'W', 2}, {'X', 1}, {'Y', 2}, {'Z', 1}, {' ', 2}
    };
    for (auto const& [letter, count] : tileCounts) {
        for (int i = 0; i < count; ++i) tileBag.push_back(letter);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tileBag.begin(), tileBag.end(), g);
}