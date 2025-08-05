// -- src/Dictionary.cpp --
#define SDL_MAIN_HANDLED
#include "Dictionary.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

Dictionary::Dictionary(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) { 
        std::cerr << "ERROR: Could not open dictionary file at " << path << std::endl;
        return; 
    }
    std::string word;
    while (file >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c){ return std::toupper(c); });
        wordList.insert(word);
    }
}
bool Dictionary::isValidWord(const std::string& word) const {
    return wordList.count(word) > 0;
}