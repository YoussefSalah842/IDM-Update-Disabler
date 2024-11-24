#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <commctrl.h>
#include <shobjidl.h>  // For file picker dialog
#pragma comment(lib, "comctl32.lib")

// Function to log errors to a file
void LogError(const std::string& errorMsg) {
    std::ofstream logFile("error_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Error: " << errorMsg << std::endl;
        logFile.close();
    }
}

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
            // Open file dialog to save the backup file
            OPENFILENAME ofn;       // common dialog box structure
            char szFile[260];       // buffer for file name

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

            if (GetSaveFileName(&ofn) == TRUE) {
                // Write the registry value to the selected file
                std::ofstream backupFile(ofn.lpstrFile);
                if (backupFile.is_open()) {
                    backupFile << "Windows Registry Editor Version 5.00\n\n";
                    backupFile << "[HKEY_CURRENT_USER\\Software\\DownloadManager]\n";
                    backupFile << "\"LstCheck\"=\"" << value << "\"\n";
                    backupFile.close();
                    MessageBox(hwnd, "Registry key backed up successfully!", "Success", MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, "Failed to create backup file.", "Error", MB_ICONERROR);
                    LogError("Failed to create backup file.");
                }
            }
        } else {
            MessageBox(hwnd, "Failed to read the registry value.", "Error", MB_ICONERROR);
            LogError("Failed to read registry value LstCheck.");
        }
        RegCloseKey(hKey);
    } else {
        MessageBox(hwnd, "Registry key does not exist.", "Error", MB_ICONERROR);
        LogError("Registry key Software\\DownloadManager does not exist.");
    }
}

// Function to disable automatic updates in the registry
void DisableUpdates(HWND hwnd) {
    HKEY hKey;
    const char* subkey = "Software\\DownloadManager";
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_WRITE, &hKey);

    if (result == ERROR_SUCCESS) {
        DWORD dwValue = 0;  // Assume 0 means updates are disabled
        result = RegSetValueEx(hKey, "DisableUpdates", 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue));

        if (result == ERROR_SUCCESS) {
            MessageBox(hwnd, "Updates have been disabled.", "Success", MB_ICONINFORMATION);
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

// Window procedure function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindow("STATIC", "IDM Updates Disable", WS_VISIBLE | WS_CHILD,
                     20, 20, 200, 25, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Click a button to perform an action", WS_VISIBLE | WS_CHILD,
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
            DisableUpdates(hwnd); // Call function to disable updates
            break;
        case 2:
            BackupRegistryKey(hwnd); // Call function to back up the registry
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
