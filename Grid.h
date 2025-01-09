#pragma once

#include <windows.h>

// The Grid class 
class Grid
{
public:
    int rows;
    int cols;
    int cellWidth;
    int cellHeight;
    int startX;
    int startY;

    Grid(int r = 4, int c = 4, int w = 79, int h = 79, int sx = 20, int sy = 20);

    int  DetectClickedRectangle(int mouseX, int mouseY) const;
    void DrawCell(HDC hdc, int row, int col,
        int number, bool isRevealed, bool isCorrect) const;
};
