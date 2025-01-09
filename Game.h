#pragma once

#include <vector>
#include <windows.h>
#include "Grid.h"

// Game Class
class Game
{
public:
    Game();

    // Shuffle or initialize game data
    bool ShuffleNumbers();  // Return 'bool' to indicate success/failure

    // Resets the grid
    void ResetGrid();

    // Render the entire grid
    void Render(HDC hdc, const Grid& grid);

    // Handle the mouse click
    void HandleClick(int mouseX, int mouseY, const Grid& grid, HWND hWnd);

    // Called when the timer elapses
    void OnTimer(HWND hWnd);

private:
    std::vector<int>  numbers;     // The 16 random numbers
    std::vector<bool> revealed;    // Reveal states
    std::vector<bool> correct;     // Correct states
    int  currentNumber;
    bool wrongClick;

    // Helpers
    bool IsValidIndex(int index) const;
};
