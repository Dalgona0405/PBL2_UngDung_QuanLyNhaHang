#include "StatisticsBLL.h"
#include <stdexcept>

Result StatisticsBLL::validateDateRange(const string &startDate, const string &endDate) {
    if (startDate.empty()) {
        return Result(false, "Ngay bat dau khong duoc de trong");
    }
    if (endDate.empty()) {
        return Result(false, "Ngay ket thuc khong duoc de trong");
    }
    if (startDate > endDate) {
        return Result(false, "Ngay bat dau phai truoc ngay ket thuc");
    }
    return Result(true, "");
}

Result StatisticsBLL::validateYear(int year) {
    if (year < 2000 || year > 2100) {
        return Result(false, "Nam khong hop le (2000-2100)");
    }
    return Result(true, "");
}

Result StatisticsBLL::validateMonth(int month) {
    if (month < 1 || month > 12) {
        return Result(false, "Thang khong hop le (1-12)");
    }
    return Result(true, "");
}

Result StatisticsBLL::validateTopN(int topN) {
    if (topN <= 0) {
        return Result(false, "So luong phai lon hon 0");
    }
    if (topN > 100) {
        return Result(false, "So luong toi da la 100");
    }
    return Result(true, "");
}

vector<RevenueByDate> StatisticsBLL::getRevenueByDate(const string &startDate, const string &endDate) {
    Result r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return vector<RevenueByDate>();
    }

    try {
        return statisticsDal.getRevenueByDate(startDate, endDate);
    } catch (const exception &e) {
        return vector<RevenueByDate>();
    }
}

vector<RevenueByMonth> StatisticsBLL::getRevenueByMonth(int year) {
    Result r = validateYear(year);
    if (!r.ok) {
        return vector<RevenueByMonth>();
    }

    try {
        return statisticsDal.getRevenueByMonth(year);
    } catch (const exception &e) {
        return vector<RevenueByMonth>();
    }
}

vector<RevenueByMonth> StatisticsBLL::getRevenueByMonthRange(int startYear, int startMonth,
                                                             int endYear, int endMonth) {
    Result r = validateYear(startYear);
    if (!r.ok) {
        return vector<RevenueByMonth>();
    }

    r = validateYear(endYear);
    if (!r.ok) {
        return vector<RevenueByMonth>();
    }

    r = validateMonth(startMonth);
    if (!r.ok) {
        return vector<RevenueByMonth>();
    }

    r = validateMonth(endMonth);
    if (!r.ok) {
        return vector<RevenueByMonth>();
    }

    if (startYear > endYear || (startYear == endYear && startMonth > endMonth)) {
        return vector<RevenueByMonth>();
    }

    try {
        return statisticsDal.getRevenueByMonthRange(startYear, startMonth, endYear, endMonth);
    } catch (const exception &e) {
        return vector<RevenueByMonth>();
    }
}

bool StatisticsBLL::getTotalRevenue(const string &startDate, const string &endDate,
                                    int &billCount, double &totalRevenue) {
    Result r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return false;
    }

    try {
        return statisticsDal.getTotalRevenue(startDate, endDate, billCount, totalRevenue);
    } catch (const exception &e) {
        return false;
    }
}

vector<TopFood> StatisticsBLL::getTopFoodsByQuantity(int topN, const string &startDate,
                                                     const string &endDate) {
    Result r = validateTopN(topN);
    if (!r.ok) {
        return vector<TopFood>();
    }

    r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return vector<TopFood>();
    }

    try {
        return statisticsDal.getTopFoodsByQuantity(topN, startDate, endDate);
    } catch (const exception &e) {
        return vector<TopFood>();
    }
}

vector<TopFood> StatisticsBLL::getTopFoodsByRevenue(int topN, const string &startDate,
                                                    const string &endDate) {
    Result r = validateTopN(topN);
    if (!r.ok) {
        return vector<TopFood>();
    }

    r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return vector<TopFood>();
    }

    try {
        return statisticsDal.getTopFoodsByRevenue(topN, startDate, endDate);
    } catch (const exception &e) {
        return vector<TopFood>();
    }
}

vector<TransactionHistory> StatisticsBLL::getTransactionHistory(const string &startDate,
                                                                const string &endDate) {
    Result r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return vector<TransactionHistory>();
    }

    try {
        return statisticsDal.getTransactionHistory(startDate, endDate);
    } catch (const exception &e) {
        return vector<TransactionHistory>();
    }
}

vector<TransactionHistory> StatisticsBLL::getTransactionHistoryByTable(int tableId,
                                                                       const string &startDate,
                                                                       const string &endDate) {
    if (tableId <= 0) {
        return vector<TransactionHistory>();
    }

    Result r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return vector<TransactionHistory>();
    }

    try {
        return statisticsDal.getTransactionHistoryByTable(tableId, startDate, endDate);
    } catch (const exception &e) {
        return vector<TransactionHistory>();
    }
}

bool StatisticsBLL::getTableStatistics(int tableId, const string &startDate, const string &endDate,
                                       int &billCount, double &totalRevenue) {
    if (tableId <= 0) {
        return false;
    }

    Result r = validateDateRange(startDate, endDate);
    if (!r.ok) {
        return false;
    }

    try {
        return statisticsDal.getTableStatistics(tableId, startDate, endDate, billCount, totalRevenue);
    } catch (const exception &e) {
        return false;
    }
}
