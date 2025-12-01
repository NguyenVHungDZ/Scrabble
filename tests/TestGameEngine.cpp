#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../include/GameEngine.h"
#include "../include/Tile.h"
#include "../include/Constants.h"

using namespace std;

// --- 1. CÁC HÀM TIỆN ÍCH (HELPERS) ---

// Hàm in log chi tiết
void logResult(string id, string desc, string input, string expected, string actual, bool passed) {
    cout << "ID:          " << id << endl;
    cout << "Description: " << desc << endl;
    cout << "Input:       " << input << endl;
    cout << "Expected:    " << expected << endl;
    cout << "Actual:      " << actual << endl;
    cout << "Status:      " << (passed ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << endl;
    cout << "--------------------------------------------------" << endl;
}

// Hàm chuẩn bị file dữ liệu giả (Môi trường test)
void setupEnvironment() {
    // 1. Tạo thư mục nếu chưa có
    system("mkdir -p tests/test_data assets");

    // 2. Tạo file từ điển giả (Dictionary Mock)
    ofstream dictFile("tests/test_data/dict_test.txt");
    dictFile << "HELLO" << endl;
    dictFile << "WORLD" << endl;
    dictFile << "TEST" << endl;
    dictFile.close();

    // 3. Tạo file highscore giả
    ofstream scoreFile("assets/highscore.txt");
    scoreFile << "0";
    scoreFile.close();
}

// Hàm giả lập hành động người chơi kéo gạch lên bàn cờ
void mockPlaceWord(GameEngine& engine, string word, int startRow, int startCol, bool horizontal) {
    for (int i = 0; i < word.length(); ++i) {
        // Tạo gạch mới với giá trị mặc định là 1 (để dễ tính điểm)
        Tile* t = new Tile(word[i], 1); 
        
        int r = horizontal ? startRow : startRow + i;
        int c = horizontal ? startCol + i : startCol;
        
        // Đặt lên bàn cờ
        engine.board->placeTemporaryTile(t, r, c);
        
        // Giả lập việc gạch rời khỏi rack (để sau này test refill)
        // Lưu ý: Logic thực tế cần lấy đúng gạch từ rack, ở đây ta mock hành vi
        if (i < engine.player->getRack().size()) {
            engine.player->removeTileFromRack(i);
        }
    }
}

// --- 2. HÀM CHẠY TEST CHÍNH ---

void runEngineTests() {
    setupEnvironment(); // Tạo file cần thiết trước khi chạy
    
    cout << "\n=== UNIT TEST REPORT: GAME ENGINE (INTEGRATION) ===\n\n";

    // ====================================================
    // NHÓM 1: KHỞI TẠO & TRẠNG THÁI (INIT & STATE)
    // ====================================================

    // --- ENG_01: INIT SUCCESS ---
    {
        GameEngine engine;
        engine.init(); // Load dictionary, init board/player
        
        bool componentsOK = (engine.board != nullptr && engine.player != nullptr && engine.dictionary != nullptr);
        
        string actual = componentsOK ? "Components Loaded" : "Null Pointer Error";
        
        logResult("ENG_01", "Khoi tao Engine va cac thanh phan", 
                  "engine.init()", "Components Loaded", actual, componentsOK);
    }

    // ====================================================
    // NHÓM 2: LOGIC GAMEPLAY (TURN LOGIC)
    // ====================================================

    // --- ENG_02: NỘP KHI BÀN CỜ TRỐNG ---
    {
        GameEngine engine;
        engine.init();
        
        TurnResult res = engine.submitWord();
        
        logResult("ENG_02", "Nop tu khi chua dat gach", 
                  "submitWord() -> Empty", "Success: False", 
                  res.success ? "Success: True" : "Success: False", !res.success);
    }

    // --- ENG_03: NỘP TỪ SAI TỪ ĐIỂN ---
    {
        GameEngine engine;
        engine.init();
        
        // Input: Từ "XYZ" (Không có trong dict_test.txt)
        mockPlaceWord(engine, "XYZ", 7, 7, true);
        
        TurnResult res = engine.submitWord();
        
        string actual = res.success ? "Success" : "Failed (Invalid Word)";
        logResult("ENG_03", "Nop tu khong hop le (Dictionary Check)", 
                  "Word: XYZ", "Failed (Invalid Word)", actual, !res.success);
    }

    // --- ENG_04: NỘP TỪ ĐÚNG & TÍNH ĐIỂM ---
    {
        GameEngine engine;
        engine.init();
        
        // Input: Từ "HELLO" (Có trong dict) đặt tại (7,7)
        // H(1)+E(1)+L(1)+L(1)+O(1) = 5. Ô (7,7) là Center (Double Word) -> 10 điểm.
        mockPlaceWord(engine, "HELLO", 7, 7, true);
        
        int scoreBefore = engine.player->getScore();
        TurnResult res = engine.submitWord();
        int scoreAfter = engine.player->getScore();
        
        bool scoreCorrect = (scoreAfter == scoreBefore + 10);
        
        string actual = "Success: " + to_string(res.success) + ", Score: " + to_string(scoreAfter);
        
        logResult("ENG_04", "Nop tu dung & Cong diem", 
                  "Word: HELLO (Center)", "Success: 1, Score: 10", actual, res.success && scoreCorrect);
    }

    // --- ENG_05: CƠ CHẾ REFILL RACK ---
    {
        GameEngine engine;
        engine.init();
        
        // Mock: Đã dùng 3 gạch để xếp chữ "THE"
        mockPlaceWord(engine, "THE", 5, 5, true);
        engine.submitWord(); // Nộp xong -> Engine tự gọi player->refillRack()
        
        // Kiểm tra xem rack có đầy lại 7 lá không
        int count = 0;
        for(Tile* t : engine.player->getRack()) {
            if (t != nullptr) count++;
        }
        
        string actual = "Tiles count: " + to_string(count);
        logResult("ENG_05", "Tu dong boc them bai (Refill)", 
                  "Submit Word -> Refill", "Tiles count: 7", actual, count == 7);
    }

    // ====================================================
    // NHÓM 3: QUẢN LÝ MẠNG & THUA CUỘC (LIVES & GAME OVER)
    // ====================================================

    // --- ENG_06: ĐỔI BÀI & TRỪ MẠNG ---
    {
        GameEngine engine;
        engine.init();
        // Mặc định Lives = 3
        
        engine.resetLetters(); // Trừ 1 mạng
        
        int lives = engine.player->getLives();
        
        // Lưu ý: Nếu logic code của bạn bị double decrement thì lives sẽ là 1 (Fail)
        // Nếu đúng thì lives = 2.
        string actual = "Lives: " + to_string(lives);
        
        logResult("ENG_06", "Doi bai tru mang (Reset Letters)", 
                  "resetLetters()", "Lives: 2", actual, lives == 2);
    }

    // --- ENG_07: ĐIỀU KIỆN GAME OVER ---
    {
        GameEngine engine;
        engine.init();
        
        // Dùng hết 3 mạng
        engine.resetLetters(); // 2
        engine.resetLetters(); // 1
        engine.resetLetters(); // 0 -> Game Over trigger
        
        bool isGameOver = (engine.getState() == EngineState::GAME_OVER);
        
        string actual = isGameOver ? "GAME_OVER" : "PLAYING";
        logResult("ENG_07", "Kiem tra Game Over", 
                  "Lives -> 0", "GAME_OVER", actual, isGameOver);
    }

    // ====================================================
    // NHÓM 4: HỆ THỐNG (SYSTEM & PERSISTENCE)
    // ====================================================

    // --- ENG_08: RESET GAME (START OVER) ---
    {
        GameEngine engine;
        engine.init();
        
        // Làm bẩn trạng thái (Tăng điểm, Game Over)
        engine.player->addScore(50);
        engine.setState(EngineState::GAME_OVER);
        
        // Act
        engine.startOver();
        
        bool resetOK = (engine.player->getScore() == 0 && engine.getState() == EngineState::PLAYING);
        
        string actual = resetOK ? "Reset OK" : "Reset Failed";
        logResult("ENG_08", "Khoi dong lai game (Start Over)", 
                  "startOver()", "Reset OK", actual, resetOK);
    }
}

int main() {
    runEngineTests();
    return 0;
}