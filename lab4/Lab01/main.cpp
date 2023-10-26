#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <cmath>
#include <sstream>
#include <string>

// Глобальные переменные
HANDLE hThread1, hThread2, hThread3;
HWND hwndProgressBar1, hwndProgressBar2, hwndProgressBar3;
HWND hwndLabel1, hwndLabel2, hwndLabel3;


// Функция потока для ProgressBar 1
bool isThread1HighPriority = false;
bool isThread2HighPriority = false;
bool isThread3HighPriority = false;

DWORD WINAPI ThreadFunc1(LPVOID lpParam) {
    SetWindowText(hwndLabel1, L"Поток 1: Низкий приоритет"); // Изначально установим низкий приоритет
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

    while (true) {
        if (isThread1HighPriority) {
            SetWindowText(hwndLabel1, L"Поток 1: Высший приоритет");
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            isThread1HighPriority = false; 
        }
        SendMessage(hwndProgressBar1, PBM_SETSTATE, PBST_NORMAL, 0);
        for (int i = 0; i <= 100; i++) {
            SendMessage(hwndProgressBar1, PBM_SETPOS, i, 0);
            Sleep(100);

            std::wstring percentText = L"Поток 1: " + std::to_wstring(i) + L"% выполнено";
            SetWindowText(hwndLabel1, percentText.c_str());

            if (GetThreadPriority(GetCurrentThread()) == THREAD_PRIORITY_HIGHEST) {
                Sleep(10);
            }
            else {
                Sleep(100); 
            }
        }
    }
    return 0;
}

DWORD WINAPI ThreadFunc2(LPVOID lpParam) {
    SetWindowText(hwndLabel2, L"Поток 2: Низкий приоритет");
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

    while (true) {

        if (isThread2HighPriority) {
            SetWindowText(hwndLabel2, L"Поток 2: Высший приоритет");
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            isThread2HighPriority = false; 
        }

        SendMessage(hwndProgressBar2, PBM_SETSTATE, PBST_NORMAL, 0);
        for (int i = 0; i <= 100; i++) {
            SendMessage(hwndProgressBar2, PBM_SETPOS, i, 0);
            Sleep(100); 

            std::wstring percentText = L"Поток 2: " + std::to_wstring(i) + L"% выполнено";
            SetWindowText(hwndLabel2, percentText.c_str());

            if (GetThreadPriority(GetCurrentThread()) == THREAD_PRIORITY_HIGHEST) {
                Sleep(10); 
            }
            else {
                Sleep(100);
            }
        }
    }
    return 0;
}

DWORD WINAPI ThreadFunc3(LPVOID lpParam) {
    SetWindowText(hwndLabel3, L"Поток 3: Низкий приоритет");
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

    while (true) {
        if (isThread3HighPriority) {
            SetWindowText(hwndLabel3, L"Поток 3: Высший приоритет");
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            isThread3HighPriority = false;
        }

        SendMessage(hwndProgressBar3, PBM_SETSTATE, PBST_NORMAL, 0);
        for (int i = 0; i <= 100; i++) {
            SendMessage(hwndProgressBar3, PBM_SETPOS, i, 0);
            Sleep(100);

            std::wstring percentText = L"Поток 3: " + std::to_wstring(i) + L"% выполнено";
            SetWindowText(hwndLabel3, percentText.c_str());

            if (GetThreadPriority(GetCurrentThread()) == THREAD_PRIORITY_HIGHEST) {
                Sleep(10);
            }
            else {
                Sleep(100);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {

        hwndProgressBar1 = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_MARQUEE, 50, 50, 200, 20,
            hwnd, NULL, NULL, NULL);
        SendMessage(hwndProgressBar1, PBM_SETMARQUEE, 1, 100);

        hwndProgressBar2 = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_MARQUEE, 50, 100, 200, 20,
            hwnd, NULL, NULL, NULL);
        SendMessage(hwndProgressBar2, PBM_SETMARQUEE, 1, 100);

        hwndProgressBar3 = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_MARQUEE, 50, 150, 200, 20,
            hwnd, NULL, NULL, NULL);
        SendMessage(hwndProgressBar3, PBM_SETMARQUEE, 1, 100);

        hwndLabel1 = CreateWindow(L"STATIC", L"Поток 1: Высший приоритет",
            WS_CHILD | WS_VISIBLE, 270, 50, 200, 20,
            hwnd, NULL, NULL, NULL);

        hwndLabel2 = CreateWindow(L"STATIC", L"Поток 2: Обычный приоритет",
            WS_CHILD | WS_VISIBLE, 270, 100, 200, 20,
            hwnd, NULL, NULL, NULL);

        hwndLabel3 = CreateWindow(L"STATIC", L"Поток 3: Низкий приоритет",
            WS_CHILD | WS_VISIBLE, 270, 150, 200, 20,
            hwnd, NULL, NULL, NULL);

        CreateWindowEx(0, L"BUTTON", L"Поток 1: Высший приоритет", WS_CHILD | WS_VISIBLE,
            50, 200, 200, 30, hwnd, (HMENU)1, NULL, NULL);

        CreateWindowEx(0, L"BUTTON", L"Поток 2: Высший приоритет", WS_CHILD | WS_VISIBLE,
            50, 240, 200, 30, hwnd, (HMENU)2, NULL, NULL);

        CreateWindowEx(0, L"BUTTON", L"Поток 3: Высший приоритет", WS_CHILD | WS_VISIBLE,
            50, 280, 200, 30, hwnd, (HMENU)3, NULL, NULL);

        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case 1:
            SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
            SetWindowText(hwndLabel1, L"Поток 1: Высший приоритет");
            SetThreadPriority(hThread2, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel2, L"Поток 2: Низший приоритет");
            SetThreadPriority(hThread3, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel3, L"Поток 3: Низший приоритет");
            break;
        case 2: 
            SetThreadPriority(hThread2, THREAD_PRIORITY_HIGHEST);
            SetWindowText(hwndLabel2, L"Поток 2: Высший приоритет");
            SetThreadPriority(hThread1, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel1, L"Поток 1: Низший приоритет");
            SetThreadPriority(hThread3, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel3, L"Поток 3: Низший приоритет");
            break;
        case 3: 
            SetThreadPriority(hThread3, THREAD_PRIORITY_HIGHEST);
            SetWindowText(hwndLabel3, L"Поток 3: Высший приоритет");
            SetThreadPriority(hThread2, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel2, L"Поток 2: Низший приоритет");
            SetThreadPriority(hThread1, THREAD_PRIORITY_LOWEST);
            SetWindowText(hwndLabel1, L"Поток 1: Низший приоритет");
            break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"WinAPIExample", NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(L"WinAPIExample", L"Пример WinAPI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    hThread1 = CreateThread(NULL, 0, ThreadFunc1, NULL, 0, NULL);
    hThread2 = CreateThread(NULL, 0, ThreadFunc2, NULL, 0, NULL);
    hThread3 = CreateThread(NULL, 0, ThreadFunc3, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    TerminateThread(hThread1, 0);
    TerminateThread(hThread2, 0);
    TerminateThread(hThread3, 0);

    return msg.wParam;
}