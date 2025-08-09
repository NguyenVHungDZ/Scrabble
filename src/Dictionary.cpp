// -- src/Dictionary.cpp --
#define SDL_MAIN_HANDLED
#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

Dictionary::Dictionary(const string& path) {
    ifstream file(path);
    if (!file.is_open()) { 
        cerr << "ERROR: Could not open dictionary file at " << path << endl;
        return; 
    }
    string word;
    while (file >> word) {
        transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c){ return toupper(c); });
        wordList.insert(word);
    }
}
bool Dictionary::isValidWord(const string& word) const {
    return wordList.count(word) > 0;
}