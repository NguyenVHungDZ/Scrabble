// -- src/Tile.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Tile.hpp"
#include "Constants.hpp"
#include "TextureManager.hpp"
#include <string>

Tile::Tile(char letter, int value, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont)
    : letter(letter), value(value), renderer(renderer), 
      rackIndex(-1), boardRow(-1), boardCol(-1),
      letterTexture(nullptr), valueTexture(nullptr) {
    
    std::string imagePath = IMAGE_ASSET_PATH + std::string(1, letter) + ".png";
    letterTexture = TextureManager::LoadImage(renderer, imagePath);
    
    std::string valueStr = std::to_string(value);
    valueTexture = TextureManager::LoadText(renderer, smallFont, valueStr, COLOR_TEXT_DARK);
}
Tile::~Tile() {
    if (letterTexture) SDL_DestroyTexture(letterTexture);
    if (valueTexture) SDL_DestroyTexture(valueTexture);
}
void Tile::render(int x, int y) {
    SDL_Rect shadowRect = {x + 2, y + 2, TILE_SIZE, TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, COLOR_TILE_SHADOW.r, COLOR_TILE_SHADOW.g, COLOR_TILE_SHADOW.b, 100);
    SDL_RenderFillRect(renderer, &shadowRect);
    
    SDL_Rect tileRect = {x, y, TILE_SIZE, TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, COLOR_TILE_NORMAL.r, COLOR_TILE_NORMAL.g, COLOR_TILE_NORMAL.b, COLOR_TILE_NORMAL.a);
    SDL_RenderFillRect(renderer, &tileRect);

    SDL_SetRenderDrawColor(renderer, COLOR_TILE_BORDER.r, COLOR_TILE_BORDER.g, COLOR_TILE_BORDER.b, COLOR_TILE_BORDER.a);
    for (int i = 0; i < 2; ++i) {
        SDL_Rect borderRect = {x + i, y + i, TILE_SIZE - (i * 2), TILE_SIZE - (i * 2)};
        SDL_RenderDrawRect(renderer, &borderRect);
    }
    
    if (letterTexture) {
        SDL_RenderCopy(renderer, letterTexture, NULL, &tileRect);
    }
    if (valueTexture) {
        int w, h; SDL_QueryTexture(valueTexture, NULL, NULL, &w, &h);
        SDL_Rect destRect = {x + TILE_SIZE - w - 4, y + TILE_SIZE - h - 4, w, h};
        SDL_RenderCopy(renderer, valueTexture, NULL, &destRect);
    }
}