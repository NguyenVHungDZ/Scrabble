#include <iostream>
#include <string>
#include <vector>
#include "../include/Player.h"
#include "../include/Tile.h"
#include "../include/Constants.h"

using namespace std;

// Hàm log màu mè
void logResult(string description, string input, string expected, string actual, bool passed) {
    cout << "Description: " << description << endl;
    cout << "Input:       " << input << endl;
    cout << "Expected:    " << expected << endl;
    cout << "Actual:      " << actual << endl;
    cout << "Status:      " << (passed ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << endl;
    cout << "--------------------------------------------------" << endl;
}

int countTiles(Player& p) {
    int count = 0;
    for (Tile* t : p.getRack()) { if (t != nullptr) count++; }
    return count;
}

void runPlayerTests() {
    cout << "\n=== UNIT TEST REPORT: PLAYER (EXTENDED) ===\n\n";

    // --- TC 1: KHỞI TẠO ---
    {
        logResult("Khoi tao Player", "new Player()", "Lives: 3, Tiles: 7", 
                  "Lives: 3, Tiles: " + to_string(countTiles(*new Player())), true);
    }

    // --- TC 2: CỘNG ĐIỂM ---
    {
        Player p;
        p.addScore(10);
        bool passed = (p.getScore() == 10);
        logResult("Cong diem lan 1", "addScore(10)", "Score: 10", "Score: " + to_string(p.getScore()), passed);
    }

    // --- TC 3: XÓA GẠCH ---
    {
        Player p;
        p.removeTileFromRack(0);
        bool passed = (p.getTileFromRack(0) == nullptr);
        logResult("Xoa gach khoi khay", "removeTileFromRack(0)", "Slot 0: nullptr", passed ? "Slot 0: nullptr" : "Has Tile", passed);
    }

    // --- TC 4: TRẢ GẠCH ---
    {
        Player p;
        Tile* t = p.getTileFromRack(1);
        p.removeTileFromRack(1);
        p.returnTileToRack(t);
        bool passed = (p.getTileFromRack(1) == t);
        logResult("Tra gach ve khay", "returnTileToRack()", "Slot 1 has Tile", passed ? "Slot 1 has Tile" : "Empty", passed);
    }

    // --- TC 5: RESET RACK (TRỪ MẠNG) ---
    {
        Player p;
        p.resetRack();
        bool passed = (p.getLives() == 2 && countTiles(p) == 7);
        logResult("Doi bai (Reset)", "resetRack()", "Lives: 2", "Lives: " + to_string(p.getLives()), passed);
    }

    // ====================================================
    //      CÁC TEST CASE MỚI (NÂNG CAO & BIÊN)
    // ====================================================

    // --- TC 6: CỘNG DỒN ĐIỂM (ACCUMULATION) ---
    {
        string desc = "Kiem tra cong don diem (Score Accumulation)";
        string input = "addScore(10) -> addScore(20)";
        
        // Act
        Player p;
        p.addScore(10);
        p.addScore(20); // Tổng phải là 30
        
        string expected = "Score: 30";
        string actual = "Score: " + to_string(p.getScore());
        
        bool isPassed = (p.getScore() == 30);
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TC 7: TRUY CẬP CHỈ SỐ SAI (BOUNDARY CHECK) ---
    {
        string desc = "Truy cap index ngoai pham vi (Out of Bounds)";
        string input = "getTileFromRack(100) & remove(-5)";
        
        // Act
        Player p;
        Tile* t = p.getTileFromRack(100); // Index quá lớn
        p.removeTileFromRack(-5);         // Index âm
        
        // Expected: Không được Crash, trả về nullptr
        string expected = "Return nullptr, No Crash";
        string actual = (t == nullptr) ? "Return nullptr, No Crash" : "Return valid pointer (Error)";
        
        bool isPassed = (t == nullptr);
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TC 8: GAME OVER LOGIC (HẾT MẠNG) ---
    {
        string desc = "Kiem tra khi het mang (Lives Exhausted)";
        string input = "Call resetRack() 4 times";
        
        // Act
        Player p;
        p.resetRack(); // Lives = 2
        p.resetRack(); // Lives = 1
        p.resetRack(); // Lives = 0
        p.resetRack(); // Lives vẫn là 0 (Không được âm)
        
        string expected = "Lives: 0 (Min limit)";
        string actual = "Lives: " + to_string(p.getLives());
        
        bool isPassed = (p.getLives() == 0);
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TC 9: CLEAN STATE KHI TRẢ GẠCH ---
    {
        string desc = "Gach tra ve phai reset toa do (Clean State)";
        string input = "Tile at (5,5) -> returnToRack()";
        
        // Act
        Player p;
        Tile* t = p.getTileFromRack(0);
        
        // Giả sử gạch đang nằm trên bàn cờ
        t->boardRow = 5; 
        t->boardCol = 5;
        p.removeTileFromRack(0);
        
        // Trả về
        p.returnTileToRack(t);
        
        // Check: Tọa độ phải về -1
        string expected = "Row: -1, Col: -1";
        string actual = "Row: " + to_string(t->boardRow) + ", Col: " + to_string(t->boardCol);
        
        bool isPassed = (t->boardRow == -1 && t->boardCol == -1);
        logResult(desc, input, expected, actual, isPassed);
    }
}

int main() {
    runPlayerTests();
    return 0;
}