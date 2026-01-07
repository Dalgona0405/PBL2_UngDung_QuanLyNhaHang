// StaffUI.h
#pragma once
#ifndef STAFF_UI_H
#define STAFF_UI_H

#include "../BLL/TableBLL.h"
#include "../BLL/UserBLL.h"
#include "ConsoleUI.h"
#include "TableUI.h"
#include <string>
#include <vector>

using namespace std;

// Menu Staff:
//  - Chon ban -> may TableListScreen(selectMode = true, handler = this)
//  - Xem thong tin ca nhan -> UserProfileScreen
//  - (Staff) Dang xuat
class StaffMenuScreen : public Screen {
    private:
        UserBLL &userBLL_;
        TableBLL &tableBLL_;

        int sel_;
        short menuStartRow_;
        vector<string> items_;

        // Dang mo ta ban dang chon (neu co)
        string currentTableInfo_;

        void buildMenu();
        void drawItem(int index, bool selected);

    public:
        explicit StaffMenuScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

#endif // STAFF_UI_H
