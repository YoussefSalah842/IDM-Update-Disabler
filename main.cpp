#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <commctrl.h>
#include <shobjidl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

std::string backupValue;  // Variable to store the backup registry value

void LogError(const std::string& errorMsg) {
    std::ofstream logFile("error_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Error: " << errorMsg << " | Error Code: " << GetLastError() << std::endl;
        logFile.close();
    }
}

void DisableUpdates(HWND hwnd, HWND statusLabel) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_WRITE, &hKey);
    if (result == ERROR_SUCCESS) {
        DWORD dwValue = 0;
        result = RegSetValueEx(hKey, "DisableUpdates", 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue));

        if (result == ERROR_SUCCESS) {
            SetWindowText(statusLabel, "Status: Updates Disabled");
            SetTextColor(GetDC(hwnd), RGB(0, 255, 0));  // Change status color to green
            InvalidateRect(hwnd, NULL, TRUE);
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

void CopyRegistryFile(HWND hwnd) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        char value[255];
        DWORD valueLength = 255;
        DWORD type;
        result = RegQueryValueEx(hKey, "DisableUpdates", NULL, &type, (LPBYTE)value, &valueLength);

        if (result == ERROR_SUCCESS) {
            OPENFILENAME ofn;
            char szFile[260];

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Registry Files\0*.reg\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (GetSaveFileName(&ofn)) {
                std::ofstream backupFile(ofn.lpstrFile);
                if (backupFile.is_open()) {
                    backupFile << "Windows Registry Editor Version 5.00\n\n";
                    backupFile << "[HKEY_CURRENT_USER\\Software\\DownloadManager]\n";
                    backupFile << "\"DisableUpdates\"=\"" << value << "\"\n";
                    backupFile.close();
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

void RestoreUpdates(HWND hwnd, HWND statusLabel) {
    OPENFILENAME ofn;
    char szFile[260];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Registry Files\0*.reg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::string command = "regedit /s " + std::string(ofn.lpstrFile);
        system(command.c_str());
        SetWindowText(statusLabel, "Status: Updates Restored");
        SetTextColor(GetDC(hwnd), RGB(0, 0, 255));  // Set status color to blue (indicating updates are running)
        InvalidateRect(hwnd, NULL, TRUE);
        MessageBox(hwnd, "Registry restored successfully!", "Success", MB_ICONINFORMATION);
    } else {
        MessageBox(hwnd, "Failed to select a registry file.", "Error", MB_ICONERROR);
        LogError("Failed to select a registry file for restoration.");
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND statusLabel;

    switch (msg) {
    case WM_CREATE: {
        CreateWindow("STATIC", "IDM Updates Disable", WS_VISIBLE | WS_CHILD,
                     100, 20, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Click a button to perform an action", WS_VISIBLE | WS_CHILD,
                     100, 50, 250, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("BUTTON", "Copy Registry File", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 80, 150, 25, hwnd, (HMENU)1, NULL, NULL);

        CreateWindow("BUTTON", "Disable Updates", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 120, 150, 25, hwnd, (HMENU)2, NULL, NULL);

        CreateWindow("BUTTON", "Restore Updates", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     100, 160, 150, 25, hwnd, (HMENU)3, NULL, NULL);

        statusLabel = CreateWindow("STATIC", "Status: Updates Running", WS_VISIBLE | WS_CHILD | SS_CENTER,
                                   100, 200, 200, 25, hwnd, NULL, NULL, NULL);
        SetTextColor(GetDC(hwnd), RGB(255, 0, 0));  // Set initial color to red
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 1:
            CopyRegistryFile(hwnd);  // Copy registry file
            break;
        case 2:
            DisableUpdates(hwnd, statusLabel);  // Disable updates
            break;
        case 3:
            RestoreUpdates(hwnd, statusLabel);  // Restore updates
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpszClassName = "RegistryTool";
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProcedure;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("RegistryTool", "IDM Updates Disable", WS_OVERLAPPED | WS_SYSMENU,
                             100, 100, 400, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
