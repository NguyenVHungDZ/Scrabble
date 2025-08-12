#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Player.h"
#include "Constants.h"
#include "TextureManager.h"
#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

Player::Player(string name, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont)
    : name(name), renderer(renderer), font(font), smallFont(smallFont), score(0), lives(3) {
    rack.resize(PLAYER_RACK_SIZE, nullptr);
}

Player::~Player() {
    for (Tile* tile : rack) {
        delete tile;
    }
    rack.clear();
}

void Player::refillRack(vector<char>& tileBag, const map<char, int>& tileValues) {
    for (int i = 0; i < PLAYER_RACK_SIZE; ++i) {
        if (rack[i] == nullptr && !tileBag.empty()) {
            char letter = tileBag.back();
            tileBag.pop_back();
            rack[i] = new Tile(letter, tileValues.at(letter), renderer, font, smallFont);
            rack[i]->rackIndex = i;
        }
    }
}

void Player::resetRack(vector<char>& tileBag) {
    for (size_t i = 0; i < rack.size(); ++i) {
        if (rack[i] != nullptr) {
            if (rack[i]->getLetter() != ' ') {
                 tileBag.push_back(rack[i]->getLetter());
            }
            delete rack[i];
            rack[i] = nullptr;
        }
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(tileBag.begin(), tileBag.end(), g);
}

void Player::renderRack(int mouseX, int mouseY, int yPos, bool isActive) {
    // --- Render Rack Background ---
    SDL_Rect rackRect = { RACK_AREA_X, yPos, BOARD_SIZE_PX, RACK_HEIGHT };
    SDL_SetRenderDrawColor(renderer, COLOR_RACK_BG.r, COLOR_RACK_BG.g, COLOR_RACK_BG.b, COLOR_RACK_BG.a);
    SDL_RenderFillRect(renderer, &rackRect);

    // --- Render Tiles ---
    int startX = RACK_AREA_X + (BOARD_SIZE_PX - (PLAYER_RACK_SIZE * (TILE_SIZE + 5))) / 2;
    int rackTileY = yPos + (RACK_HEIGHT - TILE_SIZE) / 2;
    for (size_t i = 0; i < rack.size(); ++i) {
        if (rack[i]) {
            rack[i]->render(startX + i * (TILE_SIZE + 5), rackTileY, false, mouseX, mouseY);
        }
    }

    // --- Render Inactive Overlay (Blur Effect) ---
    if (!isActive) {
        SDL_Rect overlayRect = {RACK_AREA_X, yPos, BOARD_SIZE_PX, RACK_HEIGHT};
        SDL_SetRenderDrawColor(renderer, COLOR_INACTIVE_OVERLAY.r, COLOR_INACTIVE_OVERLAY.g, COLOR_INACTIVE_OVERLAY.b, COLOR_INACTIVE_OVERLAY.a);
        SDL_RenderFillRect(renderer, &overlayRect);
    }
}

Tile* Player::getTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) {
        return rack[index];
    }
    return nullptr;
}

void Player::removeTileFromRack(int index) {
    if (index >= 0 && (size_t)index < rack.size()) {
        rack[index] = nullptr;
    }
}

void Player::returnTileToRack(Tile* tile) {
    if (tile && tile->rackIndex >= 0 && (size_t)tile->rackIndex < rack.size()) {
        if (rack[tile->rackIndex] == nullptr) {
            rack[tile->rackIndex] = tile;
        } else {
            for(size_t i = 0; i < rack.size(); ++i) {
                if(rack[i] == nullptr) {
                    rack[i] = tile;
                    tile->rackIndex = i;
                    break;
                }
            }
        }
    }
}

bool Player::hasTiles() const {
    for (const auto& tile : rack) {
        if (tile != nullptr) {
            return true;
        }
    }
    return false;
}
