#include "Game.h"
#include <algorithm>  // std::shuffle
#include <random>     // std::random_device, std::mt19937
#include <string>

// Constructor
Game::Game()
    : currentNumber(1), wrongClick(false)
{
    // Reserve space for 16 cells
    // (though we still need to properly fill them in ShuffleNumbers)
    numbers.reserve(16);
    revealed.reserve(16);
    correct.reserve(16);
}

// --------------------------------------------------
// IsValidIndex – Checks if index is within the vector range
// --------------------------------------------------
bool Game::IsValidIndex(int index) const
{
    // Ensure we're safely within the bounds of 'numbers' and other vectors
    return (index >= 0 && index < static_cast<int>(numbers.size()));
}

// --------------------------------------------------
// ShuffleNumbers
// --------------------------------------------------
bool Game::ShuffleNumbers()
{
    // Using try/catch in case of memory issues or unexpected exceptions
    try
    {
        // Clear & fill numbers [1..16]
        numbers.clear();
        for (int i = 1; i <= 16; i++)
            numbers.push_back(i);

        // Shuffle using random_device
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(numbers.begin(), numbers.end(), g);

        // Assign states
        revealed.assign(16, false);
        correct.assign(16, false);
        currentNumber = 1;
        wrongClick = false;

        // Double-check we ended with 16 items
        if (numbers.size() != 16)
        {
            // In practice, this "shouldn't happen," but good to check anyway
            return false;
        }
    }
    catch (const std::exception& e)
    {
        // If something went wrong (e.g., memory allocation failed),
        // we can log or show an error message:
        MessageBoxA(nullptr, e.what(), "Shuffle Error", MB_ICONERROR | MB_OK);
        return false;
    }
    return true; // success
}

// --------------------------------------------------
// ResetGrid
// --------------------------------------------------
void Game::ResetGrid()
{
    revealed.assign(16, false);
    correct.assign(16, false);
    currentNumber = 1;
    wrongClick = false;
}

// --------------------------------------------------
// Render
// --------------------------------------------------
void Game::Render(HDC hdc, const Grid& grid)
{
    // We'll assume grid.rows * grid.cols == 16
    int index = 0;
    for (int row = 0; row < grid.rows; ++row)
    {
        for (int col = 0; col < grid.cols; ++col)
        {
            // Though we assume 16 cells, we can still be defensive:
            if (IsValidIndex(index))
            {
                grid.DrawCell(
                    hdc, row, col,
                    numbers[index],
                    revealed[index],
                    correct[index]
                );
            }
            else
            {
                // If index is invalid, do something safe (e.g. no draw),
                // or mark an error.
                // For demonstration, we'll skip drawing.
            }
            ++index;
        }
    }
}

// --------------------------------------------------
// HandleClick
// --------------------------------------------------
void Game::HandleClick(int mouseX, int mouseY, const Grid& grid, HWND hWnd)
{
    int clickedIndex = grid.DetectClickedRectangle(mouseX, mouseY);
    if (!IsValidIndex(clickedIndex))
    {
        // It's either -1 or out of bounds
        return;
    }

    // Temporarily reveal
    revealed[clickedIndex] = true;

    // Check correctness
    if (numbers[clickedIndex] == currentNumber)
    {
        correct[clickedIndex] = true;
        currentNumber++;

        if (currentNumber > 16)
        {
            MessageBox(hWnd,
                L"Congratulations! You found all numbers in ascending order.",
                L"Success",
                MB_OK);
        }
    }
    else
    {
        // Wrong guess
        wrongClick = true;
        // Start a timer
        SetTimer(hWnd, 1, 500, NULL);
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

// --------------------------------------------------
// OnTimer
// --------------------------------------------------
void Game::OnTimer(HWND hWnd)
{
    KillTimer(hWnd, 1);
    if (wrongClick)
    {
        // For demonstration, let's do a partial check here too
        try
        {
            // We do a full reset, but it could be partial
            ResetGrid();
        }
        catch (const std::exception& e)
        {
            MessageBoxA(nullptr, e.what(), "OnTimer Error", MB_ICONERROR | MB_OK);
        }
        wrongClick = false;
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}
