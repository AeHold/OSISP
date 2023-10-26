#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <future>
#include <chrono>
#include <fstream>

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void MonitorMemoryUsage(HWND hwnd, HDC hdc);
std::wstring FormatBytes(DWORDLONG bytes);
bool exitFlag = false;
#define IDM_SAVE_RESULTS 101
std::vector<std::wstring> processInfoList;
void SaveResultsToFile();

std::wstring FormatBytes(DWORDLONG bytes)
{
    std::wstringstream ss;

    if (bytes >= 1024 * 1024 * 1024)
    {
        ss << std::fixed << std::setprecision(2) << static_cast<double>(bytes) / (1024 * 1024 * 1024) << L" GB";
    }
    else if (bytes >= 1024 * 1024)
    {
        ss << std::fixed << std::setprecision(2) << static_cast<double>(bytes) / (1024 * 1024) << L" MB";
    }
    else if (bytes >= 1024)
    {
        ss << std::fixed << std::setprecision(2) << static_cast<double>(bytes) / 1024 << L" KB";
    }
    else
    {
        ss << bytes << L" B";
    }
    return ss.str();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("MemoryMonitor"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("Memory Monitor"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreateMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_SAVE_RESULTS, L"Сохранить результаты");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Файл");
    SetMenu(hwnd, hMenu);

    std::future<void> monitorFuture;

    monitorFuture = std::async(std::launch::async, [hwnd]() {
        while (!exitFlag) {
            HDC hdc = GetDC(hwnd);
            MonitorMemoryUsage(hwnd, hdc);
            ReleaseDC(hwnd, hdc);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        });

    SetTimer(hwnd, 1, 2000, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hwnd, 1);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_TIMER:
        hdc = GetDC(hwnd);
        MonitorMemoryUsage(hwnd, hdc);
        ReleaseDC(hwnd, hdc);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        MonitorMemoryUsage(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_SAVE_RESULTS:
            SaveResultsToFile();
            break;
        }
    break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void SaveResultsToFile()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    std::wstring memoryInfo = L"Total Physical Memory: " + FormatBytes(memInfo.ullTotalPhys) + L"\n";
    memoryInfo += L" Available Physical Memory: " + FormatBytes(memInfo.ullAvailPhys) + L"\n";
    memoryInfo += L" Total Virtual Memory: " + FormatBytes(memInfo.ullTotalVirtual) + L"\n";
    memoryInfo += L" Available Virtual Memory: " + FormatBytes(memInfo.ullAvailVirtual) + L"\n";
    std::wofstream outputFile(L"E:\\OSISP\\MemoryUsageResults.txt");
    DWORD processIds[1024], bytesReturned;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        DWORD numProcesses = bytesReturned / sizeof(DWORD);
        std::vector<std::wstring> processInfoList;

        for (DWORD i = 0; i < numProcesses; i++) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
            if (hProcess) {
                TCHAR processName[MAX_PATH];
                if (GetModuleBaseName(hProcess, NULL, processName, MAX_PATH)) {
                    std::wstring processInfo = L"Process Name: " + std::wstring(processName) + L"\n";
                    PROCESS_MEMORY_COUNTERS_EX pmc;

                    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                        processInfo += L" Working Set Size: " + FormatBytes(pmc.WorkingSetSize) + L"\n";
                        processInfo += L" Private Usage: " + FormatBytes(pmc.PrivateUsage) + L"\n";
                        processInfo += L" Pagefile Usage: " + FormatBytes(pmc.PagefileUsage) + L"\n";
                    }
                    processInfoList.push_back(processInfo);
                }
                CloseHandle(hProcess);
            }
        }
        if (outputFile.is_open())
        {
            outputFile << memoryInfo << L"\n\n";

            for (const auto& processInfo : processInfoList) {
                outputFile << processInfo << L"\n\n";
            }

            outputFile.close();
            MessageBox(NULL, L"Результаты сохранены в файл 'MemoryUsageResults.txt'.", L"Информация", MB_ICONINFORMATION | MB_OK);
        }
        else {
            MessageBox(NULL, L"Не удалось создать файл для сохранения результатов.", L"Ошибка", MB_ICONERROR | MB_OK);
        }
    }
}

void MonitorMemoryUsage(HWND hwnd, HDC hdc)
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    std::wstring memoryInfo = L"Total Physical Memory: " + FormatBytes(memInfo.ullTotalPhys) + L"\n";
    memoryInfo += L" Available Physical Memory: " + FormatBytes(memInfo.ullAvailPhys) + L"\n";
    memoryInfo += L" Total Virtual Memory: " + FormatBytes(memInfo.ullTotalVirtual) + L"\n";
    memoryInfo += L" Available Virtual Memory: " + FormatBytes(memInfo.ullAvailVirtual) + L"\n";

    DWORD processIds[1024], bytesReturned;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        DWORD numProcesses = bytesReturned / sizeof(DWORD);
        std::vector<std::wstring> processInfoList;

        for (DWORD i = 0; i < numProcesses; i++) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
            if (hProcess) {
                TCHAR processName[MAX_PATH];
                if (GetModuleBaseName(hProcess, NULL, processName, MAX_PATH)) {
                    std::wstring processInfo = L"Process Name: " + std::wstring(processName) + L"\n";
                    PROCESS_MEMORY_COUNTERS_EX pmc;

                    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                        processInfo += L" Working Set Size: " + FormatBytes(pmc.WorkingSetSize) + L"\n";
                        processInfo += L" Private Usage: " + FormatBytes(pmc.PrivateUsage) + L"\n";
                        processInfo += L" Pagefile Usage: " + FormatBytes(pmc.PagefileUsage) + L"\n";
                    }
                    processInfoList.push_back(processInfo);
                }
                CloseHandle(hProcess);
            }
        }

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));
        int y = 10;
        TextOut(hdc, 10, y, memoryInfo.c_str(), static_cast<int>(memoryInfo.size()));
        y += 50;

        for (const auto& processInfo : processInfoList) {
            TextOut(hdc, 10, y, processInfo.c_str(), static_cast<int>(processInfo.size()));
            y += 50; 
        }

        ReleaseDC(hwnd, hdc);
    }
}

