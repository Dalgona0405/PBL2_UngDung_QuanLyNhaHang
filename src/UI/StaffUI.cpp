// StaffUI.cpp
#include "StaffUI.h"
#include "BillUI.h"
#include "ConsoleUtils.h"
#include "UserUI.h"
#include <conio.h>
#include <iomanip>
#include <iostream>

using namespace std;

// ======================= StaffMenuScreen =======================
StaffMenuScreen::StaffMenuScreen(App *a)
    : Screen(a),
      userBLL_(a->user()),
      tableBLL_(a->table()),
      sel_(0),
      menuStartRow_(0),
      currentTableInfo_("") {
    buildMenu();
}

void StaffMenuScreen::buildMenu() {
    items_.clear();

    items_.push_back("Chon ban");
    items_.push_back("Xem thong tin ca nhan");

    User u = userBLL_.getCurrentUser();
    if (u.roleId != 1) {
        items_.push_back("Dang xuat");
    }
    if (sel_ >= (int)items_.size())
        sel_ = (int)items_.size() - 1;
    if (sel_ < 0)
        sel_ = 0;
}

void StaffMenuScreen::drawItem(int index, bool selected) {
    short row = menuStartRow_ + index;
    gotoRC(row, 0);
    clearLine();
    cout << bullet(selected) << setw(2) << index + 1 << ". " << items_[index];
    cout.flush();
}

void StaffMenuScreen::draw() {
    cls();
    
    User u = userBLL_.getCurrentUser();
    string subtitle = "Nguoi dung: " + u.fullName + " (" + userBLL_.roleToString(u.roleId) + ")";
    printTitleBox("GIAO DIEN STAFF", subtitle);
    printDivider('=');

    if (!currentTableInfo_.empty()) {
        setColor(COLOR_MUTED);
        cout << currentTableInfo_ << "\n";
        resetColor();
    }
    cout << "\n";

    menuStartRow_ = contentRow();
    for (size_t i = 0; i < items_.size(); ++i) {
        drawItem((int)i, (int)i == sel_);
        cout << "\n";
    }

    cout << "\n";
    
    if (u.roleId == 1) {
        printHint("[Up/Down] chon  |  [Enter] thuc hien  |  [Backspace] quay lai Admin  |  [Esc] thoat");
    } else {
        printHint("[Up/Down] chon  |  [Enter] thuc hien  |  [Esc] thoat");
    }
    cout.flush();
}

void StaffMenuScreen::onKey(Key k) {
    int old = sel_;

    if (k == KEY_UP && sel_ > 0) {
        sel_--;
    } else if (k == KEY_DOWN && sel_ + 1 < (int)items_.size()) {
        sel_++;
    } else if (k == KEY_ENTER) {
        if (sel_ == 0) {
            app->push(new BillListScreen(app));
            return;
        } else if (sel_ == 1) {
            app->push(new UserProfileScreen(app));
            return;
        } else if (sel_ == 2) {
            userBLL_.logOut();
            app->replaceTop(new UserLoginScreen(app));
            return;
        }
    } else if (k == KEY_BACKSPACE) {
        User u = userBLL_.getCurrentUser();
        if (u.roleId == 1) {
            // Admin → Quay lại Admin menu
            app->pop();
            return;
        }
    } else if (k == KEY_ESC) {
        // ESC: Thoát app hoàn toàn
        app->exit();
        return;
    }
    if (sel_ != old) {
        drawItem(old, false);
        drawItem(sel_, true);
    }
}
