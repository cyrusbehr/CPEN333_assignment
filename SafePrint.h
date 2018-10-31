#pragma once
#include "..\rt.h"
#include "Shared.h"
#include "IncludeFiles.h"

#ifndef __SafePrint__
#define __SafePrint__

class SafePrint {
public:
    SafePrint();
    void sPrint(std::string str, int x, int y);

    void clearColumn(int x, int yStart);
    void drawHorizontalLine(int y);
    void drawVerticalLine(int x, int yStart);
    void drawVerticalLine(int x, int yStart, int yEnd);
    int getRowSize();
    int getColumnSize();

    std::string gradeToString(GasGrade grade);

private:
    std::mutex m_mutex;
    int m_consoleRowSize = 100;
    int m_consoleCoslumSize = 100;
};

#endif