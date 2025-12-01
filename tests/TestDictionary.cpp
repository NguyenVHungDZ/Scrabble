#include <iostream>
#include <iomanip> // Thư viện để căn chỉnh bảng (setw)
#include <vector>
#include <string>
#include "../include/Dictionary.h" // Sửa lại đường dẫn include cho đúng cấu trúc của bạn

using namespace std;

// Cấu trúc cho một Test Case
struct TestCase {
    string id;
    string input;
    bool expectedOutput;
    string description;
};

// Hàm tiện ích để in dòng kẻ ngang
void printLine() {
    cout << "--------------------------------------------------------------------------------------\n";
}

// Hàm chạy từng test case và in kết quả
void runTestSuite() {
    // 1. Setup: Load từ điển mẫu
    string dataPath = "tests/test_data/dict_test.txt";
    Dictionary dict(dataPath);

    // 2. Định nghĩa danh sách các trường hợp kiểm thử (Test Cases)
    vector<TestCase> testCases = {
        // --- Nhóm 1: Từ hợp lệ (Happy Path) ---
        {"TC01", "APPLE", true, "Tu co trong tu dien (Viet hoa)"},
        {"TC02", "Hello", true, "Tu co trong tu dien (Viet thuong -> Auto hoa)"},
        {"TC03", "cPlusPlus", true, "Tu co trong tu dien (Viet lon xon)"},
        {"TC04", "A", true, "Tu don 1 ky tu (hop le)"},

        // --- Nhóm 2: Từ không hợp lệ (Negative Cases) ---
        {"TC05", "BANANA", false, "Tu khong co trong tu dien"},
        {"TC06", "XYZ123", false, "Tu chua so hoac ky tu la"},
        
        // --- Nhóm 3: Trường hợp biên (Edge Cases) ---
        {"TC07", "", false, "Input rong (Empty string)"},
        {"TC08", "   ", false, "Input chi chua khoang trang"},
        {"TC09", "APP LE", false, "Tu co dau cach o giua"}
    };

    // 3. Header bảng báo cáo
    cout << "\n=== DICTIONARY UNIT TEST REPORT ===\n";
    cout << "Data Source: " << dataPath << "\n\n";
    
    printLine();
    cout << left 
         << setw(6) << "ID" 
         << setw(15) << "Input" 
         << setw(10) << "Expected" 
         << setw(10) << "Actual" 
         << setw(10) << "Status" 
         << "Description" << endl;
    printLine();

    // 4. Thực thi vòng lặp test
    int passed = 0;
    int failed = 0;

    for (const auto& tc : testCases) {
        // ACT: Gọi hàm cần test
        bool actualOutput = dict.isValidWord(tc.input);

        // CHECK: So sánh kết quả
        bool isPassed = (actualOutput == tc.expectedOutput);
        if (isPassed) passed++; else failed++;

        // REPORT: In ra dòng kết quả
        // Lưu ý: bool in ra là 1/0, ta đổi thành chuỗi "True"/"False" cho đẹp
        string sExpected = tc.expectedOutput ? "True" : "False";
        string sActual = actualOutput ? "True" : "False";
        string sStatus = isPassed ? "PASS" : "FAIL";

        // Tô màu đỏ nếu Fail (Mã ANSI color code)
        if (!isPassed) sStatus = "\033[1;31mFAIL\033[0m"; 
        else sStatus = "\033[1;32mPASS\033[0m";

        cout << left 
             << setw(6) << tc.id 
             << setw(15) << tc.input 
             << setw(10) << sExpected 
             << setw(10) << sActual 
             << setw(19) << sStatus // setw lớn hơn do mã màu ẩn chiếm ký tự
             << tc.description << endl;
    }
    printLine();

    // 5. Tổng kết
    cout << "SUMMARY: Total: " << testCases.size() 
         << " | Passed: " << passed 
         << " | Failed: " << failed << endl;
    
    if (failed == 0) {
        cout << "\033[1;32m>> RESULT: ALL TESTS PASSED <<\033[0m\n";
    } else {
        cout << "\033[1;31m>> RESULT: TEST SUITE FAILED <<\033[0m\n";
    }
    cout << endl;
}

int main() {
    runTestSuite();
    return 0;
}