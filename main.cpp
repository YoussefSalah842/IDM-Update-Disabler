#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <commctrl.h>
#include <shobjidl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

// Logging errors to a file
void LogError(const std::string& errorMsg) {
    std::ofstream logFile("error_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Error: " << errorMsg << " | Error Code: " << GetLastError() << std::endl;
    }
}

// Disable updates by modifying registry
void DisableUpdates(HWND hwnd, HWND statusLabel) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_SET_VALUE, &hKey);
    if (result == ERROR_SUCCESS) {
        DWORD dwValue = 1;  // Change to 1 to indicate updates are disabled
        result = RegSetValueEx(hKey, "DisableUpdates", 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue));

        if (result == ERROR_SUCCESS) {
            SetWindowText(statusLabel, "Status: Updates Disabled");
            SetTextColor(GetDC(statusLabel), RGB(0, 255, 0));  // Green for disabled
        } else {
            MessageBox(hwnd, "Failed to disable updates.", "Error", MB_ICONERROR);
            LogError("Failed to disable updates in registry.");
        }
        RegCloseKey(hKey);
    } else {
        MessageBox(hwnd, "Failed to open registry key for writing.", "Error", MB_ICONERROR);
        LogError("Failed to open registry key Software\\DownloadManager for writing.");
    }
}

// Backup registry value to a file
void CopyRegistryFile(HWND hwnd) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        DWORD value;
        DWORD valueLength = sizeof(value);
        DWORD type;

        result = RegQueryValueEx(hKey, "DisableUpdates", NULL, &type, reinterpret_cast<LPBYTE>(&value), &valueLength);

        if (result == ERROR_SUCCESS && type == REG_DWORD) {
            OPENFILENAME ofn;
            char szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Registry Files\0*.reg\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_OVERWRITEPROMPT;

            if (GetSaveFileName(&ofn)) {
                std::ofstream backupFile(ofn.lpstrFile, std::ios::out);
                if (backupFile.is_open()) {
                    backupFile << "Windows Registry Editor Version 5.00\n\n";
                    backupFile << "[HKEY_CURRENT_USER\\Software\\DownloadManager]\n";
                    backupFile << "\"DisableUpdates\"=dword:" << std::hex << value << "\n";
                    MessageBox(hwnd, "Registry key backed up successfully!", "Success", MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, "Failed to create backup file.", "Error", MB_ICONERROR);
                    LogError("Failed to create backup file.");
                }
            }
        } else {
            MessageBox(hwnd, "Failed to read the registry value.", "Error", MB_ICONERROR);
            LogError("Failed to read registry value DisableUpdates.");
        }
        RegCloseKey(hKey);
    } else {
        MessageBox(hwnd, "Registry key does not exist.", "Error", MB_ICONERROR);
        LogError("Registry key Software\\DownloadManager does not exist.");
    }
}

// Restore registry from backup file
void RestoreUpdates(HWND hwnd, HWND statusLabel) {
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Registry Files\0*.reg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::string command = "regedit /s \"" + std::string(ofn.lpstrFile) + "\"";
        if (system(command.c_str()) == 0) {
            SetWindowText(statusLabel, "Status: Updates Restored");
            SetTextColor(GetDC(statusLabel), RGB(0, 0, 255));  // Blue for restored
            MessageBox(hwnd, "Registry restored successfully!", "Success", MB_ICONINFORMATION);
        } else {
            MessageBox(hwnd, "Failed to restore the registry.", "Error", MB_ICONERROR);
            LogError("Failed to restore registry from file.");
        }
    } else {
        MessageBox(hwnd, "Failed to select a registry file.", "Error", MB_ICONERROR);
        LogError("Failed to select a registry file for restoration.");
    }
}

// Main window procedure handling button actions
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND statusLabel;

    switch (msg) {
    case WM_CREATE:
        CreateWindow("STATIC", "IDM Updates Manager", WS_VISIBLE | WS_CHILD,
                     100, 20, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("BUTTON", "Backup Registry", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 60, 150, 25, hwnd, (HMENU)1, NULL, NULL);

        CreateWindow("BUTTON", "Disable Updates", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 100, 150, 25, hwnd, (HMENU)2, NULL, NULL);

        CreateWindow("BUTTON", "Restore Updates", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 140, 150, 25, hwnd, (HMENU)3, NULL, NULL);

        statusLabel = CreateWindow("STATIC", "Status: Updates Running", WS_VISIBLE | WS_CHILD | SS_CENTER,
                                   100, 180, 200, 25, hwnd, NULL, NULL, NULL);
        SetTextColor(GetDC(statusLabel), RGB(255, 0, 0));  // Red for running
        break;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 1:
            CopyRegistryFile(hwnd);
            break;
        case 2:
            DisableUpdates(hwnd, statusLabel);
            break;
        case 3:
            RestoreUpdates(hwnd, statusLabel);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// Main application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpszClassName = "RegistryTool";
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProcedure;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindow("RegistryTool", "IDM Updates Manager",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                             NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
