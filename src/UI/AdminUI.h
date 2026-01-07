// AdminUI.h
#pragma once
#ifndef ADMIN_UI_H
#define ADMIN_UI_H

#include <string>
#include <vector>
using namespace std;

#include "../BLL/CategoryBLL.h"
#include "../BLL/FoodBLL.h"
#include "../BLL/TableBLL.h"
#include "../BLL/UserBLL.h"
#include "ConsoleUI.h"

class AdminMenuScreen : public Screen {
    private:
        FoodBLL &foodBLL_;
        CategoryBLL &catBLL_;
        TableBLL &tableBLL_;
        UserBLL &userBLL_;

        int sel_;
        short menuStartRow_;
        vector<string> items_;

        void buildMenu();
        void drawItem(int index, bool selected);

    public:
        AdminMenuScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

#endif
