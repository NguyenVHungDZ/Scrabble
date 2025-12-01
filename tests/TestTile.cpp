#include <iostream>
#include <string>
#include "../include/Tile.h"

using namespace std;

// Hàm hỗ trợ in log theo định dạng bạn yêu cầu
void logResult(string description, string input, string expected, string actual, bool passed) {
    cout << "Description: " << description << endl;
    cout << "Input:       " << input << endl;
    cout << "Expected:    " << expected << endl;
    cout << "Actual:      " << actual << endl;
    cout << "Status:      " << (passed ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << endl; // Tô màu xanh/đỏ
    cout << "--------------------------------------------------" << endl;
}

void runTileTests() {
    cout << "\n=== UNIT TEST REPORT: TILE ===\n\n";

    // --- TEST CASE 1: KHỞI TẠO CƠ BẢN ---
    {
        string desc = "Kiem tra khoi tao quan co (Constructor)";
        string input = "Tile('A', 1)";
        
        // Act
        Tile t('A', 1);
        
        // Kiểm tra Letter và Value
        string expected = "Letter: A, Value: 1";
        string actual = "Letter: " + string(1, t.getLetter()) + ", Value: " + to_string(t.getValue());
        
        bool isPassed = (t.getLetter() == 'A' && t.getValue() == 1);
        
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TEST CASE 2: GIÁ TRỊ MẶC ĐỊNH ---
    {
        string desc = "Kiem tra toa do mac dinh (chua dat)";
        string input = "New Tile()";
        
        // Act
        Tile t('Z', 10);
        
        // Mặc định row, col, rackIndex phải là -1
        string expected = "Row: -1, Col: -1, RackIdx: -1";
        string actual = "Row: " + to_string(t.boardRow) + 
                        ", Col: " + to_string(t.boardCol) + 
                        ", RackIdx: " + to_string(t.rackIndex);
        
        bool isPassed = (t.boardRow == -1 && t.boardCol == -1 && t.rackIndex == -1);
        
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TEST CASE 3: GÁN VỊ TRÍ BÀN CỜ ---
    {
        string desc = "Thay doi toa do tren ban co (Board Position)";
        string input = "t.boardRow = 7, t.boardCol = 7";
        
        // Act
        Tile t('H', 4);
        t.boardRow = 7;
        t.boardCol = 7;
        
        string expected = "Row: 7, Col: 7";
        string actual = "Row: " + to_string(t.boardRow) + ", Col: " + to_string(t.boardCol);
        
        bool isPassed = (t.boardRow == 7 && t.boardCol == 7);
        
        logResult(desc, input, expected, actual, isPassed);
    }

    // --- TEST CASE 4: GÁN VỊ TRÍ KHAY ---
    {
        string desc = "Thay doi vi tri tren khay (Rack Index)";
        string input = "t.rackIndex = 0";
        
        // Act
        Tile t('B', 3);
        t.rackIndex = 0;
        
        string expected = "RackIdx: 0";
        string actual = "RackIdx: " + to_string(t.rackIndex);
        
        bool isPassed = (t.rackIndex == 0);
        
        logResult(desc, input, expected, actual, isPassed);
    }
    
    // --- TEST CASE 5: TILE RỖNG (BLANK TILE) ---
    {
        string desc = "Kiem tra quan co trang (Blank Tile)";
        string input = "Tile(' ', 0)";
        
        // Act
        Tile t(' ', 0);
        
        string expected = "Letter: [SPACE], Value: 0";
        string letterDisplay = (t.getLetter() == ' ') ? "[SPACE]" : string(1, t.getLetter());
        string actual = "Letter: " + letterDisplay + ", Value: " + to_string(t.getValue());
        
        bool isPassed = (t.getLetter() == ' ' && t.getValue() == 0);
        
        logResult(desc, input, expected, actual, isPassed);
    }
}

int main() {
    runTileTests();
    return 0;
}