#include "Grid.h"
#include <string>
#include <sstream>

// Constructor 
Grid::Grid(int r, int c, int w, int h, int sx, int sy)
    : rows(r), cols(c), cellWidth(w), cellHeight(h), startX(sx), startY(sy)
{
}

// Detect which cell was clicked
int Grid::DetectClickedRectangle(int mouseX, int mouseY) const
{
    int row = (mouseY - startY) / cellHeight;
    int col = (mouseX - startX) / cellWidth;

    if (row >= 0 && row < rows && col >= 0 && col < cols)
    {
        return row * cols + col;
    }
    return -1;
}

// Draw a single cell
void Grid::DrawCell(HDC hdc, int row, int col,
    int number, bool isRevealed, bool isCorrect) const
{
    int x = startX + col * cellWidth;
    int y = startY + row * cellHeight;
    RECT drawRect = { x, y, x + cellWidth, y + cellHeight };

    // Choose brush color
    HBRUSH brush = nullptr;
    if (isCorrect)
        brush = CreateSolidBrush(RGB(0, 255, 0));      // Green - correct guesses
    else if (isRevealed)
        brush = CreateSolidBrush(RGB(255, 255, 0));    // Yellow - Releaving guess
    else
        brush = CreateSolidBrush(RGB(200, 200, 200));  // Gray - default grid 

    FillRect(hdc, &drawRect, brush);
    FrameRect(hdc, &drawRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    DeleteObject(brush);

    // If revealed or correct, draw the number
    if (isRevealed || isCorrect)
    {
        std::wstring numText = std::to_wstring(number);
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, numText.c_str(), -1, &drawRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}
