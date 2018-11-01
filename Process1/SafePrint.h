#pragma once
#include "..\..\rt.h"
#include "..\Shared.h"

#ifndef __SafePrint__
#define __SafePrint__

class SafePrint {
public:
    SafePrint();
    void sPrint(std::string str, int x, int y);

    void clearColumn(int x, int yStart);
    void drawHorizontalLine(int y);
    void drawVerticalLine(int x, int yStart);
    int getRowSize();
    int getColumnSize();
    void clearSection(int pumpNum);
private:
    std::mutex m_mutex;
    int m_consoleRowSize = 100;
    int m_consoleCoslumSize = 100;
};

#endif