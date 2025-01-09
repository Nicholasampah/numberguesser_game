#include <windows.h>
#include "Game.h"
#include "Grid.h"

// Global/Static
static HINSTANCE g_hInst = nullptr;
static Game      g_game;
static Grid      g_grid(4, 4, 79, 79, 20, 20);

// Forwards
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitInstance(HINSTANCE, int);
ATOM MyRegisterClass(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Register the window class
    if (!MyRegisterClass(hInstance))
    {
        // If it fails, we can show an error
        MessageBox(nullptr,
            L"Window Class Registration Failed!",
            L"Error",
            MB_ICONERROR);
        return FALSE;
    }

    // Create and show main window
    if (!InitInstance(hInstance, nCmdShow))
    {
        // Another error check
        MessageBox(nullptr,
            L"Window Creation Failed!",
            L"Error",
            MB_ICONERROR);
        return FALSE;
    }

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"NumberGuesserClass";
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance;
    HWND hWnd = CreateWindowW(L"NumberGuesserClass",
        L"Number Guesser - Error Handling Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 400,
        nullptr, nullptr,
        hInstance, nullptr);
    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Attempt to shuffle. If it fails, show an error.
        bool success = g_game.ShuffleNumbers();
        if (!success)
        {
            MessageBox(hWnd,
                L"Shuffling failed! The game might be in an unstable state.",
                L"Error",
                MB_ICONERROR);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        g_game.Render(hdc, g_grid);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_LBUTTONDOWN:
    {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);
        g_game.HandleClick(mouseX, mouseY, g_grid, hWnd);
    }
    break;

    case WM_TIMER:
        g_game.OnTimer(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
