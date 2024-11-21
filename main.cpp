#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

// Function to back up the registry key
void BackupRegistryKey(HWND hwnd) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS) {
        char value[255];
        DWORD valueLength = 255;
        DWORD type;
        result = RegQueryValueEx(hKey, "LstCheck", NULL, &type, (LPBYTE)value, &valueLength);

        if (result == ERROR_SUCCESS) {
            // Write the value to a backup file
            std::ofstream backupFile("backup.reg");
            if (backupFile.is_open()) {
                backupFile << "Windows Registry Editor Version 5.00\n\n";
                backupFile << "[HKEY_CURRENT_USER\\Software\\DownloadManager]\n";
                backupFile << "\"LstCheck\"=\"" << value << "\"\n";
                backupFile.close();
                MessageBox(hwnd, "Registry key backed up to backup.reg successfully!", "Success", MB_ICONINFORMATION);
            } else {
                MessageBox(hwnd, "Failed to create backup file.", "Error", MB_ICONERROR);
            }
        } else {
            MessageBox(hwnd, "Failed to read the registry value.", "Error", MB_ICONERROR);
        }
        RegCloseKey(hKey);
    } else {
        MessageBox(hwnd, "Registry key does not exist.", "Error", MB_ICONERROR);
    }
}

// Window procedure function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindow("STATIC", "IDM Updates Disable", WS_VISIBLE | WS_CHILD,
                     20, 20, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Click the button to disable updates", WS_VISIBLE | WS_CHILD,
                     20, 50, 250, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("BUTTON", "Disable Updates", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     20, 80, 150, 25, hwnd, (HMENU)1, NULL, NULL);

        CreateWindow("BUTTON", "Backup Registry", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     20, 120, 150, 25, hwnd, (HMENU)2, NULL, NULL);

        CreateWindow("STATIC", "Developed by YoussefSalah842", WS_VISIBLE | WS_CHILD,
                     20, 160, 200, 25, hwnd, NULL, NULL, NULL);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 1:
            // Code for disabling updates (existing functionality)
            MessageBox(hwnd, "Feature not implemented here.", "Info", MB_ICONINFORMATION);
            break;
        case 2:
            BackupRegistryKey(hwnd);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// WinMain function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpszClassName = "RegistryTool";
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProcedure;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("RegistryTool", "IDM Updates Disable", WS_OVERLAPPED | WS_SYSMENU,
                             100, 100, 400, 250, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
