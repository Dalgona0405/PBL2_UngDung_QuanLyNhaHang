#pragma once
#ifndef TABLE_UI_H
#define TABLE_UI_H

#include "../BLL/TableBLL.h"
#include "PagedListScreen.h"

class BillListScreen;

class TableListScreen : public PagedListScreen<Table> {
    private:
        TableBLL &tableBLL_;

        bool selectMode_;
        BillListScreen *billScreen_;

        bool inAction_;
        int actionIndex_;
        string msg_;
        bool showingMsg_;
        string viewNote_;
        int msgRow_;

        enum class TableFilterType { None,
                                     Number,
                                     Capacity,
                                     Status };
        enum class TableSortField { None,
                                    Number,
                                    Capacity,
                                    Status };
        enum class TableAction { Add,
                                 Search,
                                 Sort };

        TableFilterType filterType_;
        TableSortField sortField_;
        bool sortAsc_;
        int filterNumber_;
        int filterCapacity_;
        int filterStatus_;

        int topActionRow_;
        int topActionIndex_;
        bool onTopAction_;

        string statusLabel(int status) const;
        void applySort();
        void resetView();

        bool promptIntInput(const string &title, const string &label, int &out);
        bool promptSortOrder(bool &asc);
        bool promptStatus(int &status);

        void openSearchMenu();
        void openSortMenu();
        vector<TableAction> topActions() const;
        void drawTopActions();
        void handleTopAction(TableAction action);

    public:
        TableListScreen(App *a, bool selectMode = false, BillListScreen *billScreen = NULL);
        void draw() override;
        void onKey(Key k) override;
        void reloadData() override;
        void drawTableHeader() override;
        void drawRowItem(int index, const Table &item, bool selected) override;
};

class TableEditScreen : public Screen {
    private:
        TableBLL &tableBLL_;
        bool isNew_;
        Table table_;
        int focus_;
        string msg_;

        void drawForm() const;
        void editNumber(Key k);
        void editCapacity(Key k);
        void save();
        void redrawFocusLines();
        void showMsg();

    public:
        TableEditScreen(App *a, bool isNew, const Table &initial);
        ~TableEditScreen();

        void draw();
        void onKey(Key k);
};

#endif // TABLE_UI_H
