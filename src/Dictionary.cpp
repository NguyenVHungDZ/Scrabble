#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>

Dictionary::Dictionary(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        // Không in lỗi SDL, chỉ log ra console thường
        std::cout << "Warning: Could not open dictionary at " << path << std::endl;
        return;
    }
    std::string word;
    while (file >> word) {
        // Chuyển về chữ hoa để so sánh chuẩn
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);
        wordList.insert(word);
    }
}

bool Dictionary::isValidWord(const std::string& word) const {
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    return wordList.find(upperWord) != wordList.end();
}