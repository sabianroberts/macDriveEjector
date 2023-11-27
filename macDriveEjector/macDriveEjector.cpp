// macDriveEjector <--> © 2023 Sabian Roberts All Rights Reserved
//

#include <windows.h>
#include <iostream>
#include <fstream>

NOTIFYICONDATA nid;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uVersion = NOTIFYICON_VERSION;
        nid.uCallbackMessage = WM_APP;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcscpy_s(nid.szTip, L"MyApp");
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        Shell_NotifyIcon(NIM_ADD, &nid);
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            // Hide the window and remove it from the taskbar
            ShowWindow(hwnd, SW_HIDE);
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW & ~WS_EX_APPWINDOW);
        }
        else
        {
            // Show the window and add it to the taskbar
            ShowWindow(hwnd, SW_SHOW);
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_APPWINDOW & ~WS_EX_TOOLWINDOW);
        }
        break;

    case WM_APP:
        switch (lParam)
        {
        case WM_LBUTTONUP:
            ShowWindow(hwnd, SW_RESTORE);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    wchar_t CLASS_NAME[8] = L"MyClass";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"My App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

bool isDrive1Open = false;
bool isDrive2Open = false;
wchar_t driveLetter1, driveLetter2;

void toggleDrive(wchar_t driveLetter, bool& isDriveOpen) {
    wchar_t drivePath[7];
    wsprintf(drivePath, L"\\\\.\\%c:", driveLetter);

    HANDLE hDrive = CreateFile(drivePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE) {
        std::wcout << L"Failed to open drive " << driveLetter << L":" << std::endl;
        return;
    }

    DWORD bytesReturned;
    if (isDriveOpen) {
        if (!DeviceIoControl(hDrive, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
            std::wcout << L"Failed to close drive " << driveLetter << L":" << std::endl;
        }
        else {
            std::wcout << L"Closed drive " << driveLetter << L":" << std::endl;
            isDriveOpen = false;
        }
    }
    else {
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
    std::ifstream inFile("drive_letters.txt");
    if (!inFile)
    {
        std::cerr << "Unable to read drive letters";
        exit(1);
    }

    char temp;
    inFile >> temp;
    driveLetter1 = (wchar_t)toupper(temp);

    inFile >> temp;
    driveLetter2 = (wchar_t)toupper(temp);

    while (true) {
        if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F3)) {
            toggleDrive(driveLetter1, isDrive1Open); // Toggle first drive
            std::wcout << L"Ejected bay A" << std::endl;
        }
        else if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F4)) {
            toggleDrive(driveLetter2, isDrive2Open); // Toggle second drive
            std::wcout << L"Ejected bay B" << std::endl;
        }
        Sleep(100); // Sleep to reduce CPU usage
    }
    return 0;
}
