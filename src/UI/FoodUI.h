// FoodUI.h
#pragma once
#ifndef FOOD_UI_H
#define FOOD_UI_H

#include "../BLL/CategoryBLL.h"
#include "../BLL/FoodBLL.h"
#include "PagedListScreen.h"

class BillItemEditScreen;

class FoodListScreen : public PagedListScreen<Food> {
    private:
        FoodBLL &foodBLL_;
        CategoryBLL &catBLL_;

        bool selectMode_;
        BillItemEditScreen *billItemScreen_;

        bool inAction_;
        int actionIndex_;
        string msg_;
        bool showingMsg_;
        string viewNote_;

        enum class FoodFilterType { None,
                                    Name,
                                    Category,
                                    PriceRange };
        enum class FoodSortField { None,
                                   Name,
                                   Price,
                                   Category };
        enum class FoodAction { Add,
                                Search,
                                Sort };

        FoodFilterType filterType_;
        FoodSortField sortField_;
        bool sortAsc_;
        string keyword_;
        string filterCategory_;
        double minPrice_;
        double maxPrice_;

        int topActionRow_;
        int topActionIndex_;
        bool onTopAction_;
        int msgRow_;

        string buildFilterLabel() const;
        string buildSortLabel() const;
        void updateViewNoteFromState();
        string categoryNameOf(const Food &f) const;
        void applySort();
        void resetView();

        void drawOptionRow(short startRow, const vector<string> &options, int index, bool selected) const;
        void drawMenuOptions(short startRow, const vector<string> &options, int sel) const;
        bool promptInlineString(short row, const string &prefix, string &value);
        bool promptInlineInt(short row, const string &prefix, int &value);
        bool promptInlineDouble(short row, const string &prefix, double &value);

        bool promptStringInput(const string &title, const string &label, string &out);
        bool promptIntInput(const string &title, const string &label, int &out);
        bool promptDoubleInput(const string &title, const string &label, double &out);
        bool promptPriceRange(double &minPrice, double &maxPrice);
        bool promptSortOrder(bool &asc);

        void openSearchMenu();
        void openSortMenu();
        vector<FoodAction> topActions() const;
        void drawTopActions();
        void handleTopAction(FoodAction action);

    public:
        FoodListScreen(App *a, bool selectMode = false, BillItemEditScreen *billItemScreen = NULL);
        void draw() override;
        void onKey(Key k) override;
        void reloadData() override;
        void drawTableHeader() override;
        void drawRowItem(int index, const Food &item, bool selected) override;
};

class FoodEditScreen : public Screen {
        FoodBLL &foodBLL_;
        CategoryBLL &catBLL_;
        bool isNew_;
        Food food_;
        int focus_;
        string msg_;

        string categoryName(int categoryId) const;

        void drawForm() const;
        void editName(Key k);
        void editPrice(Key k);
        void chooseCategory();
        void save();
        void redrawFocusLines();
        void showMsg();

    public:
        FoodEditScreen(App *a, bool isNew, const Food &initial);

        void draw();
        void onKey(Key k);

        void setCategory(int categoryId);
};

#endif // FOOD_UI_H
