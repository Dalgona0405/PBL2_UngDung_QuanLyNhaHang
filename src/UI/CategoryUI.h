// CategoryUI.h
#pragma once
#ifndef CATEGORY_UI_H
#define CATEGORY_UI_H

#include "../BLL/CategoryBLL.h"
#include "PagedListScreen.h"
using namespace std;

class FoodEditScreen;

class CategoryListScreen : public PagedListScreen<Category> {
    private:
        CategoryBLL &catBLL_;

        bool selectMode_;
        FoodEditScreen *foodScreen_;

        bool inAction_;
        int actionIndex_;
        string msg_;
        bool showingMsg_;
        string viewNote_;
        int msgRow_;

        enum class CategoryFilterType { None,
                                        Name };
        enum class CategorySortField { None,
                                       Name };
        enum class CategoryAction { Add,
                                    Search,
                                    Sort };

        CategoryFilterType filterType_;
        CategorySortField sortField_;
        bool sortAsc_;
        string keyword_;

        int topActionRow_;
        int topActionIndex_;
        bool onTopAction_;

        string buildFilterLabel() const;
        string buildSortLabel() const;
        void updateViewNoteFromState();
        void applySort();
        void resetView();

        void drawOptionRow(short startRow, const vector<string> &options, int index, bool selected) const;
        void drawMenuOptions(short startRow, const vector<string> &options, int sel) const;
        bool promptInlineString(short row, const string &prefix, string &value);
        bool promptStringInput(const string &title, const string &label, string &out);
        bool promptSortOrder(bool &asc);

        void openSearchMenu();
        void openSortMenu();
        vector<CategoryAction> topActions() const;
        void drawTopActions();
        void handleTopAction(CategoryAction action);

    public:
        CategoryListScreen(App *a, bool selectMode = false, FoodEditScreen *foodScreen = NULL);
        void draw() override;
        void onKey(Key k) override;
        void reloadData() override;
        void drawTableHeader() override;
        void drawRowItem(int index, const Category &item, bool selected) override;
};

class CategoryEditScreen : public Screen {
        CategoryBLL &catBLL_;
        bool isNew_;
        Category cat_;
        int focus_;
        string msg_;

        void drawForm() const;
        void editName(Key k);
        void save();
        void redrawFocusLines();
        void showMsg();

    public:
        CategoryEditScreen(App *a, bool isNew, const Category &initial);

        void draw();
        void onKey(Key k);
};

#endif // CATEGORY_UI_H
