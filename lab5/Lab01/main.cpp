#include <windows.h>
#include <tchar.h>

HINSTANCE hInst;
HWND hMainWindow;
HWND hKeyEdit;
HWND hValueEdit;
HWND hOutputEdit;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateRegistryKey();
void ModifyRegistryValue();
void DeleteRegistryKey();
void ReadRegistryKey();
void ReadRegistryValue();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("RegistryUtility");
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Error"), NULL);
        return 1;
    }

    hMainWindow = CreateWindow(_T("RegistryUtility"), _T("Registry Utility"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 260, NULL, NULL, hInstance, NULL);

    if (!hMainWindow)
    {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Error"), NULL);
        return 1;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hKeyEdit = CreateWindowEx(0, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 200, 30, hWnd, (HMENU)100, hInst, NULL);
        hValueEdit = CreateWindowEx(0, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 50, 200, 30, hWnd, (HMENU)101, hInst, NULL);
        hOutputEdit = CreateWindowEx(0, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY, 10, 90, 200, 100, hWnd, (HMENU)102, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("Create Key"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 10, 150, 30, hWnd, (HMENU)200, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("Modify Value"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 50, 150, 30, hWnd, (HMENU)201, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("Delete Key"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 90, 150, 30, hWnd, (HMENU)202, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("Read Key"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 130, 150, 30, hWnd, (HMENU)203, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("Read Value"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 170, 150, 30, hWnd, (HMENU)204, hInst, NULL);
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            switch (LOWORD(wParam))
            {
            case 200:
                CreateRegistryKey();
                break;
            case 201:
                ModifyRegistryValue();
                break;
            case 202:
                DeleteRegistryKey();
                break;
            case 203:
                ReadRegistryKey();
                break;
            case 204:
                ReadRegistryValue();
                break;
            }
        }
        break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

void CreateRegistryKey()
{
    TCHAR keyName[256];
    GetWindowText(hKeyEdit, keyName, 256);
    HKEY hKey;

    if (RegCreateKey(HKEY_CURRENT_USER, keyName, &hKey) == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        MessageBox(hMainWindow, _T("Registry key created successfully."), _T("Success"), MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(hMainWindow, _T("Failed to create registry key."), _T("Error"), MB_OK | MB_ICONERROR);
    }
}

void ModifyRegistryValue()
{
    TCHAR keyName[256];
    TCHAR value[256];
    GetWindowText(hKeyEdit, keyName, 256);
    GetWindowText(hValueEdit, value, 256);

    HKEY hKey;
    if (RegOpenKey(HKEY_CURRENT_USER, keyName, &hKey) == ERROR_SUCCESS)
    {
        if (RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)value, (_tcslen(value) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            MessageBox(hMainWindow, _T("Registry value modified successfully."), _T("Success"), MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(hMainWindow, _T("Failed to modify registry value."), _T("Error"), MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        MessageBox(hMainWindow, _T("Registry key not found."), _T("Error"), MB_OK | MB_ICONERROR);
    }
}

void DeleteRegistryKey()
{
    TCHAR keyName[256];
    GetWindowText(hKeyEdit, keyName, 256);

    if (RegDeleteKey(HKEY_CURRENT_USER, keyName) == ERROR_SUCCESS)
    {
        MessageBox(hMainWindow, _T("Registry key deleted successfully."), _T("Success"), MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(hMainWindow, _T("Failed to delete registry key."), _T("Error"), MB_OK | MB_ICONERROR);
    }
}

void ReadRegistryKey()
{
    TCHAR keyName[256];
    GetWindowText(hKeyEdit, keyName, 256);

    HKEY hKey;
    if (RegOpenKey(HKEY_CURRENT_USER, keyName, &hKey) == ERROR_SUCCESS)
    {
        TCHAR outputText[1024];
        wsprintf(outputText, _T("Registry key found in HKEY_CURRENT_USER: %s"), keyName);
        SetWindowText(hOutputEdit, outputText);
        RegCloseKey(hKey);
    }
    else
    {
        TCHAR outputText[1024];
        wsprintf(outputText, _T("Registry key not found: %s"), keyName);
        SetWindowText(hOutputEdit, outputText);
    }
}

void ReadRegistryValue()
{
    TCHAR keyName[256];
    GetWindowText(hKeyEdit, keyName, 256);
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR outputText[1024];
        outputText[0] = _T('\0');  // Очищаем строку вывода

        // Получаем и выводим все значения внутри ключа (включая подключи)
        DWORD index = 0;
        TCHAR valueName[256];
        DWORD valueNameSize = sizeof(valueName);

        while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            DWORD valueType;
            DWORD valueSize;
            if (RegQueryValueEx(hKey, valueName, NULL, &valueType, NULL, &valueSize) == ERROR_SUCCESS)
            {
                // Выделяем динамически память под значение
                LPBYTE valueData = (LPBYTE)malloc(valueSize);

                if (RegQueryValueEx(hKey, valueName, NULL, &valueType, valueData, &valueSize) == ERROR_SUCCESS)
                {
                    // Добавляем значение в строку вывода
                    _tcscat_s(outputText, 1024, valueName);
                    _tcscat_s(outputText, 1024, _T(": "));

                    if (valueType == REG_SZ)
                    {
                        _tcscat_s(outputText, 1024, (LPTSTR)valueData);
                    }
                    else if (valueType == REG_DWORD)
                    {
                        DWORD value = *(DWORD*)valueData;
                        _stprintf_s(outputText, 1024, _T("%u"), value);
                    }
                    else
                    {
                        _stprintf_s(outputText, 1024, _T("Type 0x%X (not supported)"), valueType);
                    }

                    _tcscat_s(outputText, 1024, _T("\r\n"));
                }

                // Освобождаем выделенную память
                free(valueData);
            }

            // Сбрасываем размер для следующей итерации
            valueNameSize = sizeof(valueName);
            index++;
        }

        // Если строка вывода не пуста, отображаем ее
        if (_tcslen(outputText) > 0)
        {
            SetWindowText(hOutputEdit, outputText);
        }
        else
        {
            SetWindowText(hOutputEdit, _T("No values found in the registry key."));
        }

        RegCloseKey(hKey);
    }
    else
    {
        TCHAR outputText[1024];
        wsprintf(outputText, _T("Registry key not found: %s"), keyName);
        SetWindowText(hOutputEdit, outputText);
    }
}
