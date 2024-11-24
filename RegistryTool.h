#ifndef REGISTRY_TOOL_H
#define REGISTRY_TOOL_H

#include <windows.h>

// Function declarations
void BackupRegistryKey(HWND hwnd);
void DisableUpdates(HWND hwnd);
void LogError(const std::string& errorMsg);

// Registry keys and values
extern const char* REGISTRY_KEY_PATH;
extern const char* REGISTRY_VALUE_LSTCHECK;
extern const char* REGISTRY_VALUE_DISABLE_UPDATES;

#endif // REGISTRY_TOOL_H
