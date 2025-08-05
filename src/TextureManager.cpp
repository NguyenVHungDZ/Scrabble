// -- src/TextureManager.cpp --
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color color) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
    if (!textSurface) { return nullptr; }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); 
    return textTexture;
}

SDL_Texture* TextureManager::LoadImage(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "ERROR: Failed to load image " << filePath << "! IMG_Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}