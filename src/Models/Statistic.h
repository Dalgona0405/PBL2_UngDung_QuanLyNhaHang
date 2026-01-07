#ifndef STATISTIC_H
#define STATISTIC_H
#include <string>
using namespace std;

struct RevenueByDate {
        string date; // yyyy-mm-dd
        int billCount;
        double totalRevenue;
};

struct RevenueByMonth {
        int year;
        int month;
        int billCount;
        double totalRevenue;
};

struct TopFood {
        int foodId;
        string foodName;
        int totalQuantity;
        double totalRevenue;
};

struct TransactionHistory {
        int billId;
        int tableId;
        double totalPrice;
        string paidDate; // yyyy-mm-dd hh:mm:ss
        int itemCount;
};

#endif
