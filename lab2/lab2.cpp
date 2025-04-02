// Dudolin D.A. UIS-312
// Shared variable: R="ABCDE"
// Control variable: Rc
// Operation 1: when character key pressed -> Append character N to the right
// Operation 2: when digit key "1,2,...,9" pressed -> Remove N last characters
// Variant 6b

#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <conio.h>

using namespace std;

string R = "ABCDE"; // Shared variable
int delay_ms = 0;    // Operation delay
string r_contr = R;  // Control variable

int input_code = 0;

void printInfo(char key, DWORD threadId, DWORD start, DWORD end, std::string buffer) // Log output function
{
    cout << "Key pressed: " << key << endl;
    cout << "Thread ID: " << threadId << endl;
    cout << "Key pressed at: " << start << "ms" << endl;
    cout << "Thread finished at: " << end << "ms" << endl;
    cout << "Shared variable R: " << R << endl;
    cout << "Buffer variable: " << buffer << endl;
    cout << "================" << endl;
}

DWORD WINAPI deleteSymbol(LPVOID lpParam) // Function to remove characters when digits are entered
{
    DWORD start = GetTickCount();
    string buffer = R;
    if (!buffer.empty()) {
        for (size_t i = 0; i < input_code - 48 && buffer.size() > 0; i++)
        {
            buffer.pop_back();
        }
    }

    Sleep(delay_ms);

    R = buffer;

    DWORD end = GetTickCount();
    printInfo(input_code, GetCurrentThreadId(), start, end, buffer);

    return 0;
}

DWORD WINAPI addSymbol(LPVOID lpParam) // Function to add character when letters/symbols are entered
{
    DWORD start = GetTickCount();
    string buffer = R;

    buffer += static_cast<char>(input_code);

    Sleep(delay_ms);

    R = buffer;

    DWORD end = GetTickCount();
    printInfo(input_code, GetCurrentThreadId(), start, end, buffer);

    return 0;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "English");

    cout << "Enter delay in ms: ";
    cin >> delay_ms;
    cout << "You entered delay: " << delay_ms << " ms" << std::endl;

    while (true) {
        input_code = _getch();

        if ((input_code >= 49 && input_code <= 57) || ((input_code >= 58 && input_code <= 122) || input_code >= 44 && input_code <= 56))
        {
            if (input_code >= 49 && input_code <= 57) { // Digit pressed
                if (!r_contr.empty()) {
                    for (size_t i = 0; i < input_code - 48 && r_contr.size() >= 1; i++)
                    {
                        r_contr.pop_back();
                    }
                }
                cout << "================" << endl;
                cout << "Control variable: " << r_contr << endl;
                cout << "================" << endl;

                DWORD threadId;
                HANDLE threadHandle = CreateThread(NULL, 0, deleteSymbol, NULL, 0, &threadId);
            }
            else // Character/symbol pressed
            {
                r_contr += static_cast<char>(input_code);
                cout << "================" << endl;
                cout << "Control variable: " << r_contr << endl;
                cout << "================" << endl;

                DWORD threadId;
                HANDLE threadHandle = CreateThread(NULL, 0, addSymbol, NULL, 0, &threadId);
            }
        }
    }
    return 0;
}