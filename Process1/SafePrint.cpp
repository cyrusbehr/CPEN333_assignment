#pragma once
#include "SafePrint.h"

void SafePrint::sPrint(std::string str, int x, int y) {
    std::lock_guard<std::mutex> guard(m_mutex);
    MOVE_CURSOR(x, y);
    printf("%s", str.c_str());
    fflush(stdout);
}

SafePrint::SafePrint() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    m_consoleCoslumSize = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    m_consoleRowSize = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

int SafePrint::getColumnSize() {
    return m_consoleCoslumSize;
}

int SafePrint::getRowSize() {
    return m_consoleRowSize;
}

void SafePrint::drawHorizontalLine(int y) {
    for (int i = 0; i < getColumnSize(); ++i) {
        sPrint("-", i, y);
    }
}

void SafePrint::drawVerticalLine(int x, int yStart) {
    for (int i = yStart; i < getRowSize(); ++i) {
        sPrint("|", x, i);
    }
}

void SafePrint::clearColumn(int x, int yStart) {
    for (int i = yStart; i < getRowSize(); ++i) {
        sPrint("                            ", x, i); // Require exactly 28 spaces to clear row
    }
}