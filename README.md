# IDM Update Disabler

## Description
This project provides a simple tool to manage IDM updates by interacting with the Windows registry. The tool allows users to:
1. **Disable updates** by modifying the registry.
2. **Backup the registry** to a `.reg` file for future restoration.
3. **Restore registry settings** from a backup file.

## Features
- **Disable Updates**: Modifies the registry key to disable IDM updates.
- **Backup Registry**: Saves the current registry settings for IDM updates to a `.reg` file.
- **Restore Updates**: Allows users to restore IDM updates from a previously saved registry backup.

## Improvements
- **Error Logging**: All errors are logged to an `error_log.txt` file for easier debugging.
- **UI Enhancements**: Added a simple GUI with buttons to perform actions and display status updates.

## How to Use
1. **Backup the registry**: Click the "Backup Registry" button to save the current IDM registry settings.
2. **Disable updates**: Click the "Disable Updates" button to prevent IDM from updating.
3. **Restore updates**: Click the "Restore Updates" button to restore the registry to its original state.

## Requirements
- Windows 7 or Hihger
- Compiler that supports C++ and Windows API (e.g., MinGW)

## Installation
1. Download or clone this repository.
2. Open the project in your development environment (e.g., Dev-C++).
3. Build the project using the provided Makefile or directly in your IDE.
4. Run the executable.

## Known Issues
- The tool currently only works on the Windows OS and modifies registry values which should be used carefully.
