// macDriveEjector <--> © 2023-2024 Sabian Roberts All Rights Reserved
//

#include <windows.h>
#include <iostream>
#include <fstream>

bool isDrive1Open = false;
bool isDrive2Open = false;
bool isDrive3Open = false;

wchar_t driveLetter1, driveLetter2, driveLetter3;

void toggleDrive(wchar_t driveLetter, bool& isDriveOpen) {
    wchar_t drivePath[7];
    wsprintf(drivePath, L"\\\\.\\%c:", driveLetter);

    HANDLE hDrive = CreateFile(drivePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE) 
    {
        std::wcout << L"Failed to open drive " << driveLetter << L":" << std::endl;
        return;
    }

    DWORD bytesReturned;
    if (isDriveOpen) 
    {
        if (!DeviceIoControl(hDrive, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
            std::wcout << L"Failed to close drive " << driveLetter << L":" << std::endl;
        }
        else {
            std::wcout << L"Closed drive " << driveLetter << L":" << std::endl;
            isDriveOpen = false;
        }
    }
    else 
    {
        if (!DeviceIoControl(hDrive, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
            std::wcout << L"Failed to eject drive " << driveLetter << L":" << std::endl;
        }
        else {
            std::wcout << L"Ejected drive " << driveLetter << L":" << std::endl;
            isDriveOpen = true;
        }
    }

    CloseHandle(hDrive);
}

int main() {

    system("color 70");
    std::wcout << "Welcome to the macDriveEjector!" << std::endl;

    std::ifstream inFile("drive_letters.txt");
    if (!inFile)
    {
        std::cerr << "Unable to read drive letters";
        exit(1);
    }
    else
    {
        std::wcout << "Drive Letters retrieved successfully!" << std::endl;
    }

    char temp;
    inFile >> temp;
    driveLetter1 = (wchar_t)toupper(temp);

    inFile >> temp;
    driveLetter2 = (wchar_t)toupper(temp);

    inFile >> temp;
    driveLetter3 = (wchar_t)toupper(temp);

    while (true) {
        if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F2))
        {
            std::wcout << L"Ejected USB SuperDrive" << std::endl;
            toggleDrive(driveLetter1, isDrive1Open); // Toggle USB SuperDrive
            std::wcout << L"Please note that the USB SuperDrive cannot be remotely closed as it is a slimline drive." << std::endl;
        }
        else if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F3)) 
        {
            std::wcout << L"Ejected bay A" << std::endl;
            toggleDrive(driveLetter2, isDrive2Open); // Toggle first drive
        }
        else if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F4)) 
        {
            std::wcout << L"Ejected bay B" << std::endl;
            toggleDrive(driveLetter3, isDrive3Open); // Toggle second drive
        }
        Sleep(100); // Sleep to reduce CPU usage
    }
    return 0;
}
