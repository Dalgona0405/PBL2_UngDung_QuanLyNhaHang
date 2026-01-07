// ConsoleUtils.h
#pragma once
#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h>
using namespace std;

inline bool EnableVTMode() { return true; }

// Number of lines the app header occupies (keep in sync with drawAppHeader)
static const short APP_HEADER_LINES = 8;

// Simple color palette for a consistent accent across screens
static const WORD COLOR_DEFAULT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static const WORD COLOR_ACCENT = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
static const WORD COLOR_MUTED = FOREGROUND_BLUE | FOREGROUND_GREEN;
static const WORD COLOR_ALERT = FOREGROUND_RED | FOREGROUND_INTENSITY;

inline void hideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(hOut, &info))
        return;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);
}

inline void showCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(hOut, &info))
        return;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &info);
}

inline void gotoAbsRC(short row, short col) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    COORD c;
    c.X = col;
    c.Y = row;
    SetConsoleCursorPosition(hOut, c);
}

inline void gotoRC(short row, short col) {
    gotoAbsRC(row + APP_HEADER_LINES, col);
}

inline void setColor(WORD color) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    SetConsoleTextAttribute(hOut, color);
}

inline void resetColor() { setColor(COLOR_DEFAULT); }

// Lay hang hien tai (0-based)
inline short getCursorRow() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return 0;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        return 0;
    return csbi.dwCursorPosition.Y;
}

// Current row within the content area (below header), 0-based
inline short contentRow() {
    short r = getCursorRow() - APP_HEADER_LINES;
    return (r < 0) ? 0 : r;
}

inline void clearScreen() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        return;

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD home = {0, 0};

    FillConsoleOutputCharacter(hOut, ' ', cellCount, home, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, home, &count);
    SetConsoleCursorPosition(hOut, home);
}

// ConsoleUtils.h

inline void drawAppHeader() {
    clearScreen();

    gotoAbsRC(0, 0);
    const int width = 80;
    string border(width - 2, '=');

    setColor(COLOR_ACCENT);
    cout << "+" << border << "+\n";
    cout << "| " << left << setw(width - 4) << "PBL2 - HE THONG QUAN LY NHA HANG" << " |\n";
    string thinBorder(width - 4, '-');
    cout << "| " << thinBorder << " |\n";
    cout << "| " << left << setw(width - 4) << "SVTH: DaLay Hoai Linh - Thi Nguyen Thanh Truc" << " |\n";
    cout << "| " << left << setw(width - 4) << "GVHD: Truong Ngoc Chau" << " |\n";
    cout << "+" << border << "+\n";
    resetColor();
    cout << "\n";
    cout.flush();
}

inline void clearBelowHeader() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        return;

    DWORD count;
    COORD start;
    start.X = 0;
    start.Y = APP_HEADER_LINES;

    SHORT height = csbi.dwSize.Y - APP_HEADER_LINES;
    if (height <= 0)
        height = 0;

    DWORD cellCount = csbi.dwSize.X * (DWORD)height;

    FillConsoleOutputCharacter(hOut, ' ', cellCount, start, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, start, &count);
    SetConsoleCursorPosition(hOut, start);
}

inline void cls() { drawAppHeader(); }

inline void clearLine() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        return;

    DWORD count;
    COORD lineStart;
    lineStart.X = 0;
    lineStart.Y = csbi.dwCursorPosition.Y;

    FillConsoleOutputCharacter(
        hOut, ' ', csbi.dwSize.X, lineStart, &count);
    FillConsoleOutputAttribute(
        hOut, csbi.wAttributes, csbi.dwSize.X, lineStart, &count);

    SetConsoleCursorPosition(hOut, lineStart);
}

inline void printDivider(char ch = '-', int width = 70) {
    cout << string(width, ch) << "\n";
}

inline void printTitleBox(const string &title, const string &subtitle = "") {
    const int width = 70;
    string border(width - 2, '=');

    setColor(COLOR_ACCENT);
    cout << "+" << border << "+\n";
    cout << "| " << left << setw(width - 4) << title << " |\n";
    if (!subtitle.empty())
        cout << "| " << left << setw(width - 4) << subtitle << " |\n";
    cout << "+" << border << "+\n";
    resetColor();
    cout << "\n";
}

inline void printHint(const string &hint) {
    setColor(COLOR_MUTED);
    cout << hint << "\n";
    resetColor();
}

inline string bullet(bool selected) { return selected ? "> " : "  "; }

#endif // CONSOLE_UTILS_H
