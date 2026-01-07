#pragma once
#ifndef STATISTICS_UI_H
#define STATISTICS_UI_H

#include "../BLL/StatisticsBLL.h"
#include "../Models/Statistic.h"
#include "ConsoleUI.h"
#include <string>
#include <vector>

using namespace std;

enum class StatsPeriodType {
    DateRange,  // Tu ngay den ngay
    MonthRange, // Tu thang den thang
    Year        // 1 nam cu the
};

// Menu chinh cho thong ke
class StatisticsMenuScreen : public Screen {
    private:
        StatisticsBLL &statisticsBLL_;
        vector<string> items_;
        int sel_;
        int menuStartRow_;

        void buildMenu();
        void drawItem(int index, bool selected);

    public:
        explicit StatisticsMenuScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

// Thong ke doanh thu
class RevenueStatisticsScreen : public Screen {
    private:
        StatisticsBLL &statisticsBLL_;
        StatsPeriodType periodType_;
        string startDate_;
        string endDate_;
        int startYear_, startMonth_;
        int endYear_, endMonth_;

        vector<RevenueByDate> dateItems_;
        vector<RevenueByMonth> monthItems_;

        int billCount_;
        double totalRevenue_;

        int selectedIndex_;
        int firstRow_;
        string msg_;

        void reload();
        void openPeriodMenu();
        void drawDateData();
        void drawMonthData();

    public:
        explicit RevenueStatisticsScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

// Top mon an ban chay
class TopFoodsScreen : public Screen {
    private:
        StatisticsBLL &statisticsBLL_;
        StatsPeriodType periodType_;
        string startDate_;
        string endDate_;
        int startYear_, startMonth_;
        int endYear_, endMonth_;

        vector<TopFood> items_;
        int topN_;

        int selectedIndex_;
        int firstRow_;
        string msg_;

        void reload();
        void openPeriodMenu();
        void drawRow(int index, bool selected);

    public:
        explicit TopFoodsScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

// Lich su giao dich
class TransactionHistoryScreen : public Screen {
    private:
        StatisticsBLL &statisticsBLL_;
        StatsPeriodType periodType_;
        string startDate_;
        string endDate_;
        int startYear_, startMonth_;
        int endYear_, endMonth_;

        vector<TransactionHistory> allItems_;
        vector<TransactionHistory> pageItems_;

        int currentPage_;
        int totalPages_;
        static const int ITEMS_PER_PAGE = 10;

        int topActionRow_;
        int topActionIndex_;
        bool onTopAction_;

        int selectedIndex_;
        int firstRow_;
        string msg_;

        void reload();
        void loadPage();
        void openPeriodMenu();
        void drawTopActions();

    public:
        explicit TransactionHistoryScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

#endif // STATISTICS_UI_H
