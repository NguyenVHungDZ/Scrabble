// -- include/Dictionary.h --
#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <string>
#include <unordered_set>

using namespace std;
class Dictionary {
public:
    Dictionary(const string& path);
    bool isValidWord(const string& word) const;
private:
    unordered_set<string> wordList;
};
#endif 