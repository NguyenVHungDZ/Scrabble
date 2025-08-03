// -- include/Dictionary.h --
#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <string>
#include <unordered_set>
class Dictionary {
public:
    Dictionary(const std::string& path);
    bool isValidWord(const std::string& word) const;
private:
    std::unordered_set<std::string> wordList;
};
#endif // DICTIONARY_H