// -- include/TextureManager.h --
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <string>
#include "Constants.h" 

using namespace std;

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;
typedef struct TTF_Font TTF_Font;

class TextureManager {
public:
    static SDL_Texture* LoadText(SDL_Renderer* renderer, TTF_Font* font, const string& text, Color color);
    static SDL_Texture* LoadImage(SDL_Renderer* renderer, const string& filePath);
};
#endif // TEXTURE_MANAGER_H