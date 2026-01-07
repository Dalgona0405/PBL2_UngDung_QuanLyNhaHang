#ifndef STATISTICS_DAL_H
#define STATISTICS_DAL_H
#include "../Models/Statistic.h"
#include "BaseDAL.h"
#include <string>
#include <vector>

using namespace std;

class StatisticsDAL : public BaseDAL {
    private:
        void bindRevenueByDateColumns(SQLHANDLE stmt,
                                      SQLCHAR *date, size_t dateSize, SQLLEN &dateLen,
                                      SQLINTEGER &billCount, SQLLEN &billCountLen,
                                      SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen);

        void bindRevenueByMonthColumns(SQLHANDLE stmt,
                                       SQLINTEGER &year, SQLLEN &yearLen,
                                       SQLINTEGER &month, SQLLEN &monthLen,
                                       SQLINTEGER &billCount, SQLLEN &billCountLen,
                                       SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen);

        void bindTopFoodColumns(SQLHANDLE stmt,
                                SQLINTEGER &foodId, SQLLEN &foodIdLen,
                                SQLCHAR *foodName, size_t foodNameSize, SQLLEN &foodNameLen,
                                SQLINTEGER &totalQuantity, SQLLEN &totalQuantityLen,
                                SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen);

        void bindTransactionHistoryColumns(SQLHANDLE stmt,
                                           SQLINTEGER &billId, SQLLEN &billIdLen,
                                           SQLINTEGER &tableId, SQLLEN &tableIdLen,
                                           SQLDOUBLE &totalPrice, SQLLEN &totalPriceLen,
                                           SQLCHAR *paidDate, size_t paidDateSize, SQLLEN &paidDateLen,
                                           SQLINTEGER &itemCount, SQLLEN &itemCountLen);

    public:
        vector<RevenueByDate> getRevenueByDate(const string &startDate, const string &endDate);

        vector<RevenueByMonth> getRevenueByMonth(int year);
        vector<RevenueByMonth> getRevenueByMonthRange(int startYear, int startMonth, int endYear, int endMonth);

        bool getTotalRevenue(const string &startDate, const string &endDate,
                             int &billCount, double &totalRevenue);

        vector<TopFood> getTopFoodsByQuantity(int topN, const string &startDate, const string &endDate);
        vector<TopFood> getTopFoodsByRevenue(int topN, const string &startDate, const string &endDate);

        vector<TransactionHistory> getTransactionHistory(const string &startDate, const string &endDate);
        vector<TransactionHistory> getTransactionHistoryByTable(int tableId, const string &startDate, const string &endDate);

        bool getTableStatistics(int tableId, const string &startDate, const string &endDate,
                                int &billCount, double &totalRevenue);
};

#endif
