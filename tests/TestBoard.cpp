#include <iostream>
#include <string>
#include <vector>
#include "../include/Board.h"
#include "../include/Tile.h"
#include "../include/Constants.h"

using namespace std;

// Hàm in log kết quả chi tiết
void logResult(string id, string desc, string input, string expected, string actual, bool passed) {
    cout << "ID:          " << id << endl;
    cout << "Description: " << desc << endl;
    cout << "Input:       " << input << endl;
    cout << "Expected:    " << expected << endl;
    cout << "Actual:      " << actual << endl;
    cout << "Status:      " << (passed ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << endl;
    cout << "--------------------------------------------------" << endl;
}

// Helper: Chuyển Enum Bonus sang String để in log
string bonusToString(Bonus b) {
    switch(b) {
        case NONE: return "NONE";
        case TRIPLE_WORD: return "TRIPLE_WORD";
        case DOUBLE_WORD: return "DOUBLE_WORD";
        case TRIPLE_LETTER: return "TRIPLE_LETTER";
        case DOUBLE_LETTER: return "DOUBLE_LETTER";
        case CENTER: return "CENTER";
        default: return "UNKNOWN";
    }
}

void runBoardTests() {
    cout << "\n=== UNIT TEST REPORT: BOARD (CHI TIẾT) ===\n\n";

    // ====================================================
    // NHÓM 1: KHỞI TẠO & Ô THƯỞNG (BONUS SQUARES)
    // ====================================================

    // --- BRD_01: TRIPLE WORD ---
    {
        Board b;
        Bonus res = b.getBonusAt(0, 0);
        logResult("BRD_01", "Kiem tra o Triple Word tai goc (0,0)", 
                  "getBonusAt(0,0)", "TRIPLE_WORD", bonusToString(res), res == TRIPLE_WORD);
    }

    // --- BRD_02: CENTER STAR ---
    {
        Board b;
        Bonus res = b.getBonusAt(7, 7);
        logResult("BRD_02", "Kiem tra o Trung tam (Sao)", 
                  "getBonusAt(7,7)", "CENTER", bonusToString(res), res == CENTER);
    }

    // --- BRD_03: Ô THƯỜNG ---
    {
        Board b;
        Bonus res = b.getBonusAt(1, 1);
        logResult("BRD_03", "Kiem tra o thuong (Khong bonus)", 
                  "getBonusAt(1,1)", "NONE", bonusToString(res), res == NONE);
    }

    // ====================================================
    // NHÓM 2: LOGIC ĐẶT GẠCH (PLACEMENT & BOUNDARY)
    // ====================================================

    // --- BRD_04: ĐẶT HỢP LỆ ---
    {
        Board b;
        Tile t('A', 1);
        b.placeTemporaryTile(&t, 5, 5);
        bool occupied = b.isOccupied(5, 5);
        Tile* fetched = b.getTileAt(5, 5);
        
        string actual = (occupied && fetched == &t) ? "Occupied by A" : "Empty/Wrong Tile";
        logResult("BRD_04", "Dat gach vao o hop le", 
                  "placeTile(A, 5, 5)", "Occupied by A", actual, occupied && fetched == &t);
    }

    // --- BRD_05: OUT OF BOUNDS (NEGATIVE) ---
    {
        Board b;
        Tile t('A', 1);
        b.placeTemporaryTile(&t, -1, 0); // Index âm
        bool occupied = b.isOccupied(-1, 0); 
        // Logic Board: Nếu out of bounds -> isOccupied trả về true (Blocked)
        
        logResult("BRD_05", "Chan dat gach index am (Negative)", 
                  "placeTile(A, -1, 0)", "Blocked (True)", occupied ? "Blocked (True)" : "Allowed (False)", occupied == true);
    }

    // --- BRD_06: OUT OF BOUNDS (OVERFLOW) ---
    {
        Board b;
        Tile t('A', 1);
        b.placeTemporaryTile(&t, 20, 20); // Index quá lớn
        bool occupied = b.isOccupied(20, 20);
        
        logResult("BRD_06", "Chan dat gach index qua lon", 
                  "placeTile(A, 20, 20)", "Blocked (True)", occupied ? "Blocked (True)" : "Allowed (False)", occupied == true);
    }

    // --- BRD_07: ĐẶT ĐÈ (OVERLAP) ---
    {
        Board b;
        Tile t1('A', 1);
        Tile t2('B', 2);
        
        b.placeTemporaryTile(&t1, 5, 5);
        b.finalizeTurn(); // Chốt t1
        
        // Cố tình đặt t2 vào chỗ t1
        b.placeTemporaryTile(&t2, 5, 5);
        
        Tile* current = b.getTileAt(5, 5);
        string actual = (current == &t1) ? "Giu Nguyen (A)" : "Bi De (B)";
        
        logResult("BRD_07", "Ngan chan dat de len gach cu", 
                  "place(B) on top of (A)", "Giu Nguyen (A)", actual, current == &t1);
    }

    // ====================================================
    // NHÓM 3: XỬ LÝ TỪ (WORD FORMATION)
    // ====================================================

    // --- BRD_08: TỪ HÀNG NGANG ---
    {
        Board b;
        Tile t1('H', 4); Tile t2('I', 1);
        b.placeTemporaryTile(&t1, 7, 7);
        b.placeTemporaryTile(&t2, 7, 8);
        
        WordPlacement wp = b.getPlacedWord();
        string actual = wp.isValid ? "Valid: " + wp.word : "Invalid";
        
        logResult("BRD_08", "Ghep tu hang ngang", 
                  "Tiles at (7,7), (7,8)", "Valid: HI", actual, wp.isValid && wp.word == "HI");
    }

    // --- BRD_09: TỪ HÀNG DỌC ---
    {
        Board b;
        Tile t1('U', 1); Tile t2('P', 3);
        b.placeTemporaryTile(&t1, 7, 7);
        b.placeTemporaryTile(&t2, 8, 7);
        
        WordPlacement wp = b.getPlacedWord();
        string actual = wp.isValid ? "Valid: " + wp.word : "Invalid";
        
        logResult("BRD_09", "Ghep tu hang doc", 
                  "Tiles at (7,7), (8,7)", "Valid: UP", actual, wp.isValid && wp.word == "UP");
    }

    // --- BRD_10: TỪ RỜI RẠC (GAP ERROR) ---
    {
        Board b;
        Tile t1('A', 1); Tile t2('B', 3);
        b.placeTemporaryTile(&t1, 7, 7);
        b.placeTemporaryTile(&t2, 7, 9); // Cách 1 ô (Lỗi)
        
        WordPlacement wp = b.getPlacedWord();
        // Lưu ý: Nếu logic code Board chưa check gap, test này sẽ FAIL -> Giúp phát hiện lỗi
        string actual = wp.isValid ? "Valid (Loi Logic)" : "Invalid (Chuan)";
        
        logResult("BRD_10", "Phat hien tu roi rac (Gap)", 
                  "Tiles at (7,7) & (7,9)", "Invalid (Chuan)", actual, !wp.isValid);
    }

    // ====================================================
    // NHÓM 4: TÍNH ĐIỂM (SCORING)
    // ====================================================

    // --- BRD_11: ĐIỂM CƠ BẢN ---
    {
        Board b;
        Tile t1('A', 1);
        b.placeTemporaryTile(&t1, 1, 1); // Ô thường
        WordPlacement wp = b.getPlacedWord();
        int score = b.calculateScore(wp);
        
        logResult("BRD_11", "Tinh diem co ban (Khong bonus)", 
                  "Tile A(1) at (1,1)", "Score: 1", "Score: " + to_string(score), score == 1);
    }

    // --- BRD_12: DOUBLE LETTER ---
    {
        Board b;
        Tile t1('K', 5);
        b.placeTemporaryTile(&t1, 0, 3); // Ô Double Letter
        WordPlacement wp = b.getPlacedWord();
        int score = b.calculateScore(wp); // 5 * 2 = 10
        
        logResult("BRD_12", "Tinh diem Double Letter", 
                  "Tile K(5) at (0,3)", "Score: 10", "Score: " + to_string(score), score == 10);
    }

    // --- BRD_13: TRIPLE WORD ---
    {
        Board b;
        Tile t1('N', 1); Tile t2('O', 1);
        b.placeTemporaryTile(&t1, 0, 0); // Triple Word
        b.placeTemporaryTile(&t2, 0, 1);
        
        WordPlacement wp = b.getPlacedWord();
        int score = b.calculateScore(wp); // (1+1) * 3 = 6
        
        logResult("BRD_13", "Tinh diem Triple Word", 
                  "NO(2) at Triple Word", "Score: 6", "Score: " + to_string(score), score == 6);
    }

    // ====================================================
    // NHÓM 5: QUẢN LÝ LƯỢT (GAME FLOW)
    // ====================================================

    // --- BRD_14: THU HỒI GẠCH (RECALL) ---
    {
        Board b;
        vector<Tile*> rack(7, nullptr);
        Tile t('Z', 10);
        t.rackIndex = 2;
        rack[2] = &t;
        
        b.placeTemporaryTile(&t, 7, 7);
        b.recallTiles(rack);
        
        bool cleanBoard = !b.isOccupied(7, 7);
        bool resetPos = (t.boardRow == -1);
        
        string actual = (cleanBoard && resetPos) ? "Clean & Reset" : "Dirty/Wrong Pos";
        logResult("BRD_14", "Thu hoi gach ve khay (Recall)", 
                  "recallTiles()", "Clean & Reset", actual, cleanBoard && resetPos);
    }
}

int main() {
    runBoardTests();
    return 0;
}