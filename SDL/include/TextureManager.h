// -- include/TextureManager.h --
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <string>
#include "Constants.h" // For the Color struct

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;
typedef struct TTF_Font TTF_Font; // Corrected forward declaration

class TextureManager {
public:
    static SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color color);
};
#endif // TEXTURE_MANAGER_H