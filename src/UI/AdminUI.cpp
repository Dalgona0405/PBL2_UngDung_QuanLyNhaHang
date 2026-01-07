// AdminUI.cpp
#include "AdminUI.h"
#include "CategoryUI.h"
#include "ConsoleUtils.h"
#include "FoodUI.h"
#include "StaffUI.h"
#include "StatisticsUI.h"
#include "TableUI.h"
#include "UserUI.h"
#include <iomanip>
#include <iostream>
using namespace std;

AdminMenuScreen::AdminMenuScreen(App *a)
    : Screen(a),
      foodBLL_(a->food()),
      catBLL_(a->category()),
      tableBLL_(a->table()),
      userBLL_(a->user()),
      sel_(0),
      menuStartRow_(0) {
    buildMenu();
}

void AdminMenuScreen::buildMenu() {
    items_.clear();
    // 0
    items_.push_back("Quan ly mon an");
    // 1
    items_.push_back("Quan ly danh muc");
    // 2
    items_.push_back("Quan ly ban an");
    // 3
    items_.push_back("Quan ly nhan su");
    // 4
    items_.push_back("Thong ke & Bao cao");
    // 5
    items_.push_back("Mo giao dien Staff");
    // 6
    items_.push_back("Dang xuat");

    if (sel_ >= (int)items_.size())
        sel_ = (int)items_.size() - 1;
}

void AdminMenuScreen::drawItem(int index, bool selected) {
    short row = menuStartRow_ + index;
    gotoRC(row, 0);
    clearLine();
    cout << bullet(selected) << setw(2) << index + 1 << ". " << items_[index];
    cout.flush();
}

void AdminMenuScreen::draw() {
    cls();
    User u = userBLL_.getCurrentUser();

    string subtitle = "Nguoi dung: " + u.fullName + " (" + userBLL_.roleToString(u.roleId) + ")";
    printTitleBox("MENU QUAN LY ADMIN", subtitle);
    printDivider('=');
    cout << "Chon tac vu:\n";

    menuStartRow_ = contentRow();

    for (size_t i = 0; i < items_.size(); ++i) {
        drawItem((int)i, (int)i == sel_);
        cout << "\n";
    }

    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] vao muc  |  [Esc] thoat app");
    cout.flush();
}

void AdminMenuScreen::onKey(Key k) {
    int old = sel_;
    if (k == KEY_UP && sel_ > 0) {
        sel_--;
    } else if (k == KEY_DOWN && sel_ + 1 < (int)items_.size()) {
        sel_++;
    } else if (k == KEY_ENTER) {
        if (sel_ == 0) {
            // Quan ly mon an: vao list truc tiep
            app->push(new FoodListScreen(app));
            return;
        } else if (sel_ == 1) {
            // Quan ly danh muc
            app->push(new CategoryListScreen(app, false, NULL));
            return;
        } else if (sel_ == 2) {
            // Quan ly ban an
            app->push(new TableListScreen(app));
            return;
        } else if (sel_ == 3) {
            app->push(new UserListScreen(app));
            return;
        } else if (sel_ == 4) {
            // Thong ke & Bao cao
            app->push(new StatisticsMenuScreen(app));
            return;
        } else if (sel_ == 5) {
            // Mo giao dien Staff (Admin view staff)
            app->push(new StaffMenuScreen(app));
            return;
        } else if (sel_ == 6) {
            // Dang xuat + quay ve man hinh dang nhap
            userBLL_.logOut();
            app->replaceTop(new UserLoginScreen(app));
            return;
        }
    }

    if (sel_ != old) {
        drawItem(old, false);
        drawItem(sel_, true);
    }
}
