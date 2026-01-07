// ConsoleInput.h
#pragma once
#ifndef CONSOLE_INPUT_H
#define CONSOLE_INPUT_H

#include <conio.h>

enum Key {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_ESC,
    KEY_OTHER
};

inline Key readKey() {
    int c = _getch();
    if (c == 0 || c == 224) {
        int c2 = _getch();
        switch (c2) {
        case 72:
            return KEY_UP;
        case 80:
            return KEY_DOWN;
        case 75:
            return KEY_LEFT;
        case 77:
            return KEY_RIGHT;
        default:
            return KEY_OTHER;
        }
    }
    switch (c) {
    case 13:
        return KEY_ENTER;
    case 27:
        return KEY_ESC;
    case 8:
        return KEY_BACKSPACE;
    default:
        return KEY_OTHER;
    }
}

#endif // CONSOLE_INPUT_H
