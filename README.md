# IDM Update Disabler

IDM Updates Disabler is a lightweight tool designed to disable automatic updates for Internet Download Manager

This project allows you to interact with the Windows registry to manage the update settings for DownloadManager. It provides functionality to back up disable and restore registry values related to the software.

![Screenshot 2024-12-02 183141](https://github.com/user-attachments/assets/323db905-8181-4294-ae68-e990013d864e)

user-friendly tool crafted to help you manage your Internet Download Manager (IDM) settings with ease. With just a click of a button, this little utility disables IDM’s automatic update feature, giving you more control over your software. It’s a neat, no-fuss solution for users who prefer to avoid unwanted updates. Developed with care and simplicity in mind.

## Features

1. **Copy Registry File**:  
   - Back up the current registry value for `DisableUpdates` in the registry key `Software\DownloadManager` to a `.reg` file.
   
2. **Disable Updates**:  
   - Disable updates by modifying the `DisableUpdates` registry key. After disabling updates, the status changes to "Updates Disabled" in green.
   
3. **Restore Updates**:  
   - Restore the registry value for `DisableUpdates` from a previously saved `.reg` file.

4. **Dynamic Status Label**:  
   - The status label shows the current state of updates.
     - "Updates Running" (Red) when updates are active.
     - "Updates Disabled" (Green) after the disable action is performed.

## How to Use

1. **Copy Registry File**:  
   - Click the **Copy Registry File** button to back up the current registry value to a `.reg` file.
   
2. **Disable Updates**:  
   - Click the **Disable Updates** button to modify the registry and disable updates. The status will change to "Updates Disabled" in green.

3. **Restore Updates**:  
   - Click the **Restore Updates** button to restore the original registry value from a previously backed-up `.reg` file.

## Known Issues

- The program requires administrative privileges to modify the registry.
- The "Restore Updates" function requires a valid backup `.reg` file to work.


![Screenshot 2024-11-20 201446](https://github.com/user-attachments/assets/bea3e2f6-8192-4dcb-99ae-7ea5f09ef6a8)

You will notice that the program will no longer give you an update message. To see the effect, go to

HKEY_CURRENT_USER > Software > DownloadManager.

In the registry, you will find the change that was made to the LstCheck file.
