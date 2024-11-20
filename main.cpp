#include <windows.h>
#include <iostream>
#include <string>
#include <shellapi.h>  


void DisableIDMUpdates() {
    HKEY hKey;
    LONG result;

    
    result = RegOpenKeyEx(HKEY_CURRENT_USER, 
                          "Software\\DownloadManager", 
                          0, 
                          KEY_WRITE, 
                          &hKey);

    if (result == ERROR_SUCCESS) {
        
        const char* value = "01/06/99";
        result = RegSetValueEx(hKey, 
                               "LstCheck", 
                               0, 
                               REG_SZ, 
                               (const BYTE*)value, 
                               strlen(value) + 1);

        if (result == ERROR_SUCCESS) {
            MessageBox(NULL, "Program updates have been successfully disabled.", "Success", MB_OK);
        } else {
            MessageBox(NULL, "Failed to disable updates.", "Error", MB_OK);
        }

        RegCloseKey(hKey);
    } else {
        MessageBox(NULL, "Failed to open the registry key.", "Error", MB_OK);
    }
}


void OpenGitHubLink() {
    ShellExecute(0, "open", "https://github.com/YoussefSalah842", 0, 0, SW_SHOWNORMAL);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { 
                DisableIDMUpdates();
            } else if (LOWORD(wParam) == 2) { 
                OpenGitHubLink();
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    
    const char* className = "IDMUpdateDisabler";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    RegisterClass(&wc);

    
    HWND hwnd = CreateWindowEx(0, className, "IDM Updates Disable", 
                               WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 
                               350, 250, NULL, NULL, wc.hInstance, NULL);

    
    CreateWindowEx(0, "STATIC", "IDM Updates Disable", WS_CHILD | WS_VISIBLE, 
                   50, 20, 250, 20, hwnd, NULL, wc.hInstance, NULL);

    
    CreateWindowEx(0, "BUTTON", "Disable IDM Updates", WS_CHILD | WS_VISIBLE, 
                   50, 60, 250, 30, hwnd, (HMENU)1, wc.hInstance, NULL);

    
    CreateWindowEx(0, "STATIC", "Developed by YoussefSalah842", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
                   50, 120, 250, 20, hwnd, (HMENU)2, wc.hInstance, NULL);

    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

