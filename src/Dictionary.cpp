// -- src/Dictionary.cpp --
#define SDL_MAIN_HANDLED
#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

/**
 * @brief Hàm khởi tạo (Constructor) cho lớp Dictionary.
 * Đọc các từ từ một file văn bản và nạp chúng vào bộ nhớ.
 * @param path Đường dẫn đến file từ điển (ví dụ: "assets/dictionary.txt").
 */
Dictionary::Dictionary(const string& path) {
    // Mở file từ điển để đọc
    ifstream file(path);

    // Kiểm tra xem file có mở thành công không
    if (!file.is_open()) { 
        // Nếu không, in ra thông báo lỗi và thoát
        cerr << "ERROR: Could not open dictionary file at " << path << endl;
        return; 
    }

    string word;
    // Đọc từng từ trong file cho đến khi hết file
    while (file >> word) {
        // Chuyển đổi từ vừa đọc được thành chữ hoa (UPPERCASE)
        // Điều này giúp việc so sánh từ không phân biệt chữ hoa/thường
        transform(word.begin(), word.end(), word.begin(),
                  [](unsigned char c){ return toupper(c); });
        
        // Chèn từ đã được chuyển đổi vào wordList (một unordered_set)
        // unordered_set đảm bảo tra cứu từ rất nhanh và không có từ nào bị trùng lặp.
        wordList.insert(word);
    }
}

/**
 * @brief Kiểm tra xem một từ có hợp lệ (có trong từ điển) hay không.
 * @param word Từ cần kiểm tra.
 * @return true nếu từ có trong từ điển, false nếu không.
 */
bool Dictionary::isValidWord(const string& word) const {
    // Sử dụng hàm count() của unordered_set để kiểm tra sự tồn tại của từ.
    // Hàm này trả về 1 nếu từ tồn tại và 0 nếu không.
    // Đây là cách tra cứu rất hiệu quả.
    return wordList.count(word) > 0;
}
