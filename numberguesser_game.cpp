// numberguesser_game.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "framework.h"
#include "numberguesser_game.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> // For std::shuffle
#include <random>    // For std::random_device and std::mt19937
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
vector<int> numbers;       // Holds the numbers for the grid
vector<bool> revealed;     // Tracks which rectangles are revealed
vector<bool> correct;      // Tracks which numbers have been correctly clicked
int currentNumber = 1;     // Tracks the current number in the sequence
// Add global or static variables to track the state
static bool g_wrongClick = false;
static const UINT_PTR TIMER_ID = 1;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void ShuffleNumbers(); // Function to reshuffle numbers
void DrawMyRectangle(HDC hdc, RECT rect, int number, bool isRevealed, bool isCorrect);
int DetectClickedRectangle(int x, int y, int gridRows, int gridCols, int rectWidth, int rectHeight, int startX, int startY);
void ResetGrid(); // Resets the grid visibility


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_NUMBERGUESSERGAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NUMBERGUESSERGAME));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NUMBERGUESSERGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_NUMBERGUESSERGAME);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

// Function to shuffle numbers
void ShuffleNumbers() {
	numbers.clear();
	for (int n = 1; n <= 16; ++n) {
		numbers.push_back(n);
	}
	random_device rd;
	mt19937 g(rd());
	shuffle(numbers.begin(), numbers.end(), g);

	revealed.assign(16, false); // Set all rectangles to hidden
	correct.assign(16, false); // Mark all rectangles as not correct
	currentNumber = 1;         // Reset the sequence
}

// Function to reset the grid visibility
void ResetGrid() {
	revealed.assign(16, false);
	correct.assign(16, false); // Mark all as not correct
	currentNumber = 1;         // Reset the current number to 1
}

// Detect Clicked Rectangle
int DetectClickedRectangle(int x, int y, int gridRows, int gridCols, int rectWidth, int rectHeight, int startX, int startY) {
	int col = (y - startX) / rectWidth;
	int row = (x - startY) / rectHeight;

	if (col >= 0 && col < gridCols && row >= 0 && row < gridRows) {
		return row * gridCols + col;
	}

	return -1;
}

typedef struct RECTA
{
	LONG    x;
	LONG    y;
	LONG    rectHeight;
	LONG    rectWidth;
};

void DrawMyRectangle(HDC hdc, RECTA rect, int number, bool isRevealed, bool isCorrect) {
	// Adjust the rectangle parameters for drawing
	RECT drawRect = { rect.x, rect.y, rect.rectWidth, rect.rectHeight };

	// Select the appropriate brush color
	HBRUSH brush;
	if (isCorrect) {
		brush = CreateSolidBrush(RGB(0, 255, 0)); // Green for correct numbers
	}
	else if (isRevealed) {
		brush = CreateSolidBrush(RGB(255, 255, 0)); // Yellow for revealed numbers
	}
	else {
		brush = CreateSolidBrush(RGB(200, 200, 200)); // Gray for hidden numbers
	}

	// Fill the rectangle with the selected color
	FillRect(hdc, &drawRect, brush);
	FrameRect(hdc, &drawRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	DeleteObject(brush);

	// Draw the number if the rectangle is revealed or correct
	if (isRevealed || isCorrect) {
		wstring numText = std::to_wstring(number);
		SetBkMode(hdc, TRANSPARENT); // Ensure the text background is transparent
		DrawText(hdc, numText.c_str(), -1, &drawRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		ShuffleNumbers(); // Initialize the grid
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...

		int index = 0; // Initialize index to track the current number
		int x = 20, y = 20, rectWidth = 79, rectHeight = 79;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				RECTA rect = { x, y,y + rectHeight, x + rectWidth };
				DrawMyRectangle(hdc, rect, numbers[index], revealed[index], correct[index]);
				index++;
				y += rectHeight - 1;
			}
			y = 20;
			x += rectWidth - 1;
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		int gridRows = 4, gridCols = 4, rectWidth = 79, rectHeight = 79, startX = 20, startY = 20;

		// Get the index of the clicked rectangle
		int clickedIndex = DetectClickedRectangle(mouseX, mouseY, gridRows, gridCols, rectWidth, rectHeight, startX, startY);

		if (clickedIndex != -1) { // If a valid rectangle is clicked
			revealed[clickedIndex] = true; // Temporarily reveal the clicked rectangle
			InvalidateRect(hWnd, nullptr, TRUE);


			if (numbers[clickedIndex] == currentNumber) {
				// Correct click: Reveal the rectangle and mark it as correct
				correct[clickedIndex] = true;
				currentNumber++; // Move to the next number in the sequence

				// If all 16 numbers are clicked in correct order:
				if (currentNumber > 16)
				{
					// Display a success message
					MessageBox(hWnd,
						L"Congratulations! You have correctly clicked all numbers in ascending order.",
						L"Success",
						MB_OK);
				}
			}
			else {
				// Wrong click: Show the clicked number briefly

				// Wrong click
				g_wrongClick = true;

				// Start a 500 ms timer so the user can see the revealed number
				SetTimer(hWnd, TIMER_ID, 500, NULL);

			}
			InvalidateRect(hWnd, nullptr, TRUE); // Force redraw after updating the grid state
		}
	}
	break;
	case WM_TIMER:
	{
		if (wParam == TIMER_ID)
		{
			KillTimer(hWnd, TIMER_ID);

			// After 500ms, hides all numbers (including correct ones)
			if (g_wrongClick)
			{
				ResetGrid();           // hides everything but keeps positions
				g_wrongClick = false;
				InvalidateRect(hWnd, nullptr, TRUE);
			}
		}
	}
	break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
