#pragma once
#ifndef BILL_UI_H
#define BILL_UI_H

#include "../BLL/BillBLL.h"
#include "../BLL/FoodBLL.h"
#include "../BLL/TableBLL.h"
#include "ConsoleUI.h"
#include <string>
#include <vector>

using namespace std;

class BillListScreen : public Screen {
    private:
        BillBLL &billBLL_;
        FoodBLL &foodBLL_;
        TableBLL &tableBLL_;

        vector<BillItem> items_;
        Bill bill_;
        double total_;

        Table table_;
        bool hasTable_;
        bool autoChooseTable_;

        int selectedIndex_;
        bool inAction_;
        int actionIndex_;
        string msg_;
        bool showingMsg_;
        int msgRow_;

        int firstRow_;
        int addRow_;

        void reload();
        void drawRow(int index, bool selected);

        void chooseTable();

    public:
        explicit BillListScreen(App *a);

        void draw() override;
        void onKey(Key k) override;

        void setTable(const Table &t);
};

class BillItemEditScreen : public Screen {
    private:
        BillBLL &billBLL_;
        FoodBLL &foodBLL_;

        Table table_;
        BillItem item_;
        bool isNew_;

        Food food_;
        bool hasFood_;
        int quantity_;
        string note_;
        int focus_;
        string msg_;

        void drawForm() const;
        void redrawFocusLines();
        void showMsg();

        void chooseFood();
        void editQuantity(Key k);
        void editNote(Key k);
        void save();

    public:
        BillItemEditScreen(App *a, const Table &table, const BillItem &initial, bool isNew);

        void draw() override;
        void onKey(Key k) override;

        void setFood(const Food &f);
};

#endif // BILL_UI_H
