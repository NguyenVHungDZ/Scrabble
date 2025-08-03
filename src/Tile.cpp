// -- src/Tile.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "Tile.h"
#include "Constants.h"
#include "TextureManager.h"
#include <string>

Tile::Tile(char letter, int value, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont)
    : letter(letter), value(value), renderer(renderer), 
      rackIndex(-1), boardRow(-1), boardCol(-1),
      letterTexture(nullptr), valueTexture(nullptr) {
    
    std::string letterStr(1, letter);
    letterTexture = TextureManager::LoadText(renderer, font, letterStr, COLOR_TEXT);
    
    std::string valueStr = std::to_string(value);
    valueTexture = TextureManager::LoadText(renderer, smallFont, valueStr, COLOR_TEXT);
}
Tile::~Tile() {
    if (letterTexture) SDL_DestroyTexture(letterTexture);
    if (valueTexture) SDL_DestroyTexture(valueTexture);
}
void Tile::render(int x, int y) {
    SDL_Rect tileRect = {x, y, TILE_SIZE, TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, COLOR_TILE_NORMAL.r, COLOR_TILE_NORMAL.g, COLOR_TILE_NORMAL.b, COLOR_TILE_NORMAL.a);
    SDL_RenderFillRect(renderer, &tileRect);
    SDL_SetRenderDrawColor(renderer, COLOR_TILE_BORDER.r, COLOR_TILE_BORDER.g, COLOR_TILE_BORDER.b, COLOR_TILE_BORDER.a);
    SDL_RenderDrawRect(renderer, &tileRect);
    if (letterTexture) {
        int w, h;
        SDL_QueryTexture(letterTexture, NULL, NULL, &w, &h);
        SDL_Rect destRect = {x + (TILE_SIZE - w) / 2, y + (TILE_SIZE - h) / 2 - 2, w, h};
        SDL_RenderCopy(renderer, letterTexture, NULL, &destRect);
    }
    if (valueTexture) {
        int w, h;
        SDL_QueryTexture(valueTexture, NULL, NULL, &w, &h);
        SDL_Rect destRect = {x + TILE_SIZE - w - 4, y + TILE_SIZE - h - 4, w, h};
        SDL_RenderCopy(renderer, valueTexture, NULL, &destRect);
    }
}