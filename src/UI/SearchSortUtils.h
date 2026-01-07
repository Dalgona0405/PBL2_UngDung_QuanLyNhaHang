// SearchSortUtils.h
#pragma once
#ifndef SEARCH_SORT_UTILS_H
#define SEARCH_SORT_UTILS_H

#include "ConsoleInput.h"
#include "ConsoleUtils.h"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// Use Key/readKey from ConsoleInput.h
struct SearchState {
        string note;            // rendered view note
        string keyword;         // free text keyword
        bool hasFilter = false; // whether a filter is active
};

template <typename FIELD_ENUM>
struct SortState {
        FIELD_ENUM field; // enum type for sort field
        bool asc = true;  // ascending order
};

// Generic inline text prompt used by search screens
// Returns: 1=Enter confirmed, 0=Backspace cancel (stay), -1=ESC exit
inline int promptInlineText(int row, const string &label, string &value) {
    string buffer = value;
    showCursor();
    while (true) {
        gotoRC(row, 0);
        clearLine();
        cout << bullet(true) << label << " : " << buffer;
        cout.flush();
        int ch = _getch();
        if (ch == 13) { // Enter
            value = buffer;
            hideCursor();
            return 1;
        }
        if (ch == 27) { // ESC exits immediately
            hideCursor();
            return -1;
        }
        if (ch == 8) { // Backspace deletes char or cancels if empty
            if (!buffer.empty()) {
                buffer.pop_back();
                continue;
            }
            // Do not clear line here; let caller redraw original label
            hideCursor();
            return 0;
        }
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (isprint(static_cast<unsigned char>(ch)))
            buffer.push_back(static_cast<char>(ch));
    }
}

// Inline integer prompt
// Returns: 1=Enter confirmed, 0=Backspace cancel (stay), -1=ESC exit
inline int promptInlineInt(int row, const string &label, int &value) {
    string buffer = (value == 0) ? string() : to_string(value);
    showCursor();
    while (true) {
        gotoRC(row, 0);
        clearLine();
        cout << bullet(true) << label << " : " << buffer;
        cout.flush();
        int ch = _getch();
        if (ch == 13) { // Enter
            value = buffer.empty() ? 0 : atoi(buffer.c_str());
            hideCursor();
            return 1;
        }
        if (ch == 27) { // ESC exits immediately
            hideCursor();
            return -1;
        }
        if (ch == 8) { // Backspace deletes char or cancels if empty
            if (!buffer.empty()) {
                buffer.pop_back();
                continue;
            }
            // Do not clear line here; let caller redraw original label
            hideCursor();
            return 0;
        }
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (isdigit(static_cast<unsigned char>(ch)))
            buffer.push_back(static_cast<char>(ch));
    }
}

// Inline double prompt (0 decimal default)
// Returns: 1=Enter confirmed, 0=Backspace cancel (stay), -1=ESC exit
inline int promptInlineDouble(int row, const string &label, double &value) {
    string buffer;
    if (value != 0) {
        stringstream ss;
        ss << fixed << setprecision(0) << value;
        buffer = ss.str();
    }
    showCursor();
    while (true) {
        gotoRC(row, 0);
        clearLine();
        cout << bullet(true) << label << " : " << buffer;
        cout.flush();
        int ch = _getch();
        if (ch == 13) { // Enter
            try {
                value = buffer.empty() ? 0 : stod(buffer);
            } catch (...) {
                value = 0;
            }
            hideCursor();
            return 1;
        }
        if (ch == 27) { // ESC exits immediately
            hideCursor();
            return -1;
        }
        if (ch == 8) { // Backspace deletes char or cancels if empty
            if (!buffer.empty()) {
                buffer.pop_back();
                continue;
            }
            // Do not clear line here; let caller redraw original label
            hideCursor();
            return 0;
        }
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
            if (ch == '.' && buffer.find('.') != string::npos)
                continue;
            buffer.push_back(static_cast<char>(ch));
        }
    }
}

// Render a simple vertical selection menu and return selected index or -1 if canceled
inline int selectMenu(const vector<string> &options, const string &title) {
    cls();
    printTitleBox(title);
    printDivider('=');
    short startRow = contentRow();
    int sel = 0;

    auto redrawItem = [&](int index, bool selected) {
        gotoRC(startRow + index, 0);
        clearLine();
        cout << bullet(selected) << options[index];
    };

    for (size_t i = 0; i < options.size(); ++i) {
        cout << bullet((int)i == sel) << options[i] << "\n";
    }
    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] xac nhan  |  [Backspace]/[Esc] huy");
    cout.flush();
    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC)
            return -1;
        if (k == KEY_UP && sel > 0) {
            redrawItem(sel, false);
            sel--;
            redrawItem(sel, true);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            redrawItem(sel, false);
            sel++;
            redrawItem(sel, true);
        } else if (k == KEY_ENTER) {
            return sel;
        }
    }
}

#endif // SEARCH_SORT_UTILS_H
