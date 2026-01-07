#ifndef STATISTICS_BLL_H
#define STATISTICS_BLL_H
#include "../DAL/StatisticsDAL.h"
#include "../Models/Statistic.h"
#include "Result.h"
#include <string>
#include <vector>

using namespace std;

class StatisticsBLL {
    private:
        StatisticsDAL &statisticsDal;

        Result validateDateRange(const string &startDate, const string &endDate);
        Result validateYear(int year);
        Result validateMonth(int month);
        Result validateTopN(int topN);

    public:
        StatisticsBLL(StatisticsDAL &sDal) : statisticsDal(sDal) {}

        vector<RevenueByDate> getRevenueByDate(const string &startDate, const string &endDate);

        vector<RevenueByMonth> getRevenueByMonth(int year);
        vector<RevenueByMonth> getRevenueByMonthRange(int startYear, int startMonth,
                                                      int endYear, int endMonth);

        bool getTotalRevenue(const string &startDate, const string &endDate,
                             int &billCount, double &totalRevenue);

        vector<TopFood> getTopFoodsByQuantity(int topN, const string &startDate, const string &endDate);
        vector<TopFood> getTopFoodsByRevenue(int topN, const string &startDate, const string &endDate);

        vector<TransactionHistory> getTransactionHistory(const string &startDate, const string &endDate);
        vector<TransactionHistory> getTransactionHistoryByTable(int tableId, const string &startDate,
                                                                const string &endDate);

        bool getTableStatistics(int tableId, const string &startDate, const string &endDate,
                                int &billCount, double &totalRevenue);
};

#endif
