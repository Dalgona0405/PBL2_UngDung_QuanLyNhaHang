#include "StatisticsDAL.h"
#include <iostream>
using namespace std;

void StatisticsDAL::bindRevenueByDateColumns(SQLHANDLE stmt,
                                             SQLCHAR *date, size_t dateSize, SQLLEN &dateLen,
                                             SQLINTEGER &billCount, SQLLEN &billCountLen,
                                             SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen) {
    bindColumnString(stmt, 1, date, dateSize, dateLen);
    bindColumnInt(stmt, 2, billCount, billCountLen);
    bindColumnDouble(stmt, 3, totalRevenue, totalRevenueLen);
}

void StatisticsDAL::bindRevenueByMonthColumns(SQLHANDLE stmt,
                                              SQLINTEGER &year, SQLLEN &yearLen,
                                              SQLINTEGER &month, SQLLEN &monthLen,
                                              SQLINTEGER &billCount, SQLLEN &billCountLen,
                                              SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen) {
    bindColumnInt(stmt, 1, year, yearLen);
    bindColumnInt(stmt, 2, month, monthLen);
    bindColumnInt(stmt, 3, billCount, billCountLen);
    bindColumnDouble(stmt, 4, totalRevenue, totalRevenueLen);
}

void StatisticsDAL::bindTopFoodColumns(SQLHANDLE stmt,
                                       SQLINTEGER &foodId, SQLLEN &foodIdLen,
                                       SQLCHAR *foodName, size_t foodNameSize, SQLLEN &foodNameLen,
                                       SQLINTEGER &totalQuantity, SQLLEN &totalQuantityLen,
                                       SQLDOUBLE &totalRevenue, SQLLEN &totalRevenueLen) {
    bindColumnInt(stmt, 1, foodId, foodIdLen);
    bindColumnString(stmt, 2, foodName, foodNameSize, foodNameLen);
    bindColumnInt(stmt, 3, totalQuantity, totalQuantityLen);
    bindColumnDouble(stmt, 4, totalRevenue, totalRevenueLen);
}

void StatisticsDAL::bindTransactionHistoryColumns(SQLHANDLE stmt,
                                                  SQLINTEGER &billId, SQLLEN &billIdLen,
                                                  SQLINTEGER &tableId, SQLLEN &tableIdLen,
                                                  SQLDOUBLE &totalPrice, SQLLEN &totalPriceLen,
                                                  SQLCHAR *paidDate, size_t paidDateSize, SQLLEN &paidDateLen,
                                                  SQLINTEGER &itemCount, SQLLEN &itemCountLen) {
    bindColumnInt(stmt, 1, billId, billIdLen);
    bindColumnInt(stmt, 2, tableId, tableIdLen);
    bindColumnDouble(stmt, 3, totalPrice, totalPriceLen);
    bindColumnString(stmt, 4, paidDate, paidDateSize, paidDateLen);
    bindColumnInt(stmt, 5, itemCount, itemCountLen);
}

vector<RevenueByDate> StatisticsDAL::getRevenueByDate(const string &startDate, const string &endDate) {
    vector<RevenueByDate> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT CONVERT(DATE, paid_date) AS date, "
            "       COUNT(*) AS billCount, "
            "       SUM(total_price) AS totalRevenue "
            "FROM bills "
            "WHERE paid_date IS NOT NULL AND paid_date BETWEEN ? AND ? "
            "GROUP BY CONVERT(DATE, paid_date) "
            "ORDER BY date";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare revenue by date fail");

        SQLCHAR start[32], end[32];
        SQLLEN startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindString(stmt.get(), 1, start, sizeof(start), startInd);
        bindString(stmt.get(), 2, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute revenue by date fail");

        SQLCHAR date[32];
        SQLINTEGER billCount;
        SQLDOUBLE totalRevenue;
        SQLLEN dateLen, billCountLen, totalRevenueLen;

        bindRevenueByDateColumns(stmt.get(), date, sizeof(date), dateLen,
                                 billCount, billCountLen, totalRevenue, totalRevenueLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            RevenueByDate item;
            item.date = getString(date, dateLen);
            item.billCount = billCount;
            item.totalRevenue = totalRevenue;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getRevenueByDate error: " << e.what() << endl;
    }

    return items;
}

vector<RevenueByMonth> StatisticsDAL::getRevenueByMonth(int year) {
    vector<RevenueByMonth> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT YEAR(paid_date) AS year, "
            "       MONTH(paid_date) AS month, "
            "       COUNT(*) AS billCount, "
            "       SUM(total_price) AS totalRevenue "
            "FROM bills "
            "WHERE paid_date IS NOT NULL AND YEAR(paid_date) = ? "
            "GROUP BY YEAR(paid_date), MONTH(paid_date) "
            "ORDER BY year, month";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare revenue by month fail");

        SQLINTEGER y = year;
        SQLLEN yInd;
        bindInt(stmt.get(), 1, y, yInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute revenue by month fail");

        SQLINTEGER yr, mth, billCount;
        SQLDOUBLE totalRevenue;
        SQLLEN yrLen, mthLen, billCountLen, totalRevenueLen;

        bindRevenueByMonthColumns(stmt.get(), yr, yrLen, mth, mthLen,
                                  billCount, billCountLen, totalRevenue, totalRevenueLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            RevenueByMonth item;
            item.year = yr;
            item.month = mth;
            item.billCount = billCount;
            item.totalRevenue = totalRevenue;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getRevenueByMonth error: " << e.what() << endl;
    }

    return items;
}

vector<RevenueByMonth> StatisticsDAL::getRevenueByMonthRange(int startYear, int startMonth,
                                                             int endYear, int endMonth) {
    vector<RevenueByMonth> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT YEAR(paid_date) AS year, "
            "       MONTH(paid_date) AS month, "
            "       COUNT(*) AS billCount, "
            "       SUM(total_price) AS totalRevenue "
            "FROM bills "
            "WHERE paid_date IS NOT NULL "
            "  AND (YEAR(paid_date) * 12 + MONTH(paid_date)) >= ? "
            "  AND (YEAR(paid_date) * 12 + MONTH(paid_date)) <= ? "
            "GROUP BY YEAR(paid_date), MONTH(paid_date) "
            "ORDER BY year, month";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare revenue by month range fail");

        SQLINTEGER start = startYear * 12 + startMonth;
        SQLINTEGER end = endYear * 12 + endMonth;
        SQLLEN startInd, endInd;

        bindInt(stmt.get(), 1, start, startInd);
        bindInt(stmt.get(), 2, end, endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute revenue by month range fail");

        SQLINTEGER yr, mth, billCount;
        SQLDOUBLE totalRevenue;
        SQLLEN yrLen, mthLen, billCountLen, totalRevenueLen;

        bindRevenueByMonthColumns(stmt.get(), yr, yrLen, mth, mthLen,
                                  billCount, billCountLen, totalRevenue, totalRevenueLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            RevenueByMonth item;
            item.year = yr;
            item.month = mth;
            item.billCount = billCount;
            item.totalRevenue = totalRevenue;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getRevenueByMonthRange error: " << e.what() << endl;
    }

    return items;
}

bool StatisticsDAL::getTotalRevenue(const string &startDate, const string &endDate,
                                    int &billCount, double &totalRevenue) {
    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT COUNT(*) AS billCount, "
            "       COALESCE(SUM(total_price), 0) AS totalRevenue "
            "FROM bills "
            "WHERE paid_date IS NOT NULL AND paid_date BETWEEN ? AND ?";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare total revenue fail");

        SQLCHAR start[32], end[32];
        SQLLEN startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindString(stmt.get(), 1, start, sizeof(start), startInd);
        bindString(stmt.get(), 2, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute total revenue fail");

        SQLINTEGER bc;
        SQLDOUBLE tr;
        SQLLEN bcLen, trLen;

        bindColumnInt(stmt.get(), 1, bc, bcLen);
        bindColumnDouble(stmt.get(), 2, tr, trLen);

        if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            billCount = bc;
            totalRevenue = tr;
            return true;
        }
    } catch (const exception &e) {
        cout << "getTotalRevenue error: " << e.what() << endl;
    }

    return false;
}

vector<TopFood> StatisticsDAL::getTopFoodsByQuantity(int topN, const string &startDate,
                                                     const string &endDate) {
    vector<TopFood> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT TOP (?) f.food_id, f.name, "
            "       SUM(bi.quantity) AS totalQuantity, "
            "       SUM(bi.sub_total) AS totalRevenue "
            "FROM bill_items bi "
            "INNER JOIN bills b ON bi.bill_id = b.bill_id "
            "INNER JOIN foods f ON bi.food_id = f.food_id "
            "WHERE b.paid_date IS NOT NULL AND b.paid_date BETWEEN ? AND ? "
            "GROUP BY f.food_id, f.name "
            "ORDER BY totalQuantity DESC";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare top foods by quantity fail");

        SQLINTEGER n = topN;
        SQLCHAR start[32], end[32];
        SQLLEN nInd, startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindInt(stmt.get(), 1, n, nInd);
        bindString(stmt.get(), 2, start, sizeof(start), startInd);
        bindString(stmt.get(), 3, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute top foods by quantity fail");

        SQLINTEGER foodId, totalQuantity;
        SQLCHAR foodName[256];
        SQLDOUBLE totalRevenue;
        SQLLEN foodIdLen, foodNameLen, totalQuantityLen, totalRevenueLen;

        bindTopFoodColumns(stmt.get(), foodId, foodIdLen, foodName, sizeof(foodName), foodNameLen,
                           totalQuantity, totalQuantityLen, totalRevenue, totalRevenueLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            TopFood item;
            item.foodId = foodId;
            item.foodName = getString(foodName, foodNameLen);
            item.totalQuantity = totalQuantity;
            item.totalRevenue = totalRevenue;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getTopFoodsByQuantity error: " << e.what() << endl;
    }

    return items;
}

vector<TopFood> StatisticsDAL::getTopFoodsByRevenue(int topN, const string &startDate,
                                                    const string &endDate) {
    vector<TopFood> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT TOP (?) f.food_id, f.food_name, "
            "       SUM(bi.quantity) AS totalQuantity, "
            "       SUM(bi.sub_total) AS totalRevenue "
            "FROM bill_items bi "
            "INNER JOIN bills b ON bi.bill_id = b.bill_id "
            "INNER JOIN foods f ON bi.food_id = f.food_id "
            "WHERE b.paid_date IS NOT NULL AND b.paid_date BETWEEN ? AND ? "
            "GROUP BY f.food_id, f.food_name "
            "ORDER BY totalRevenue DESC";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare top foods by revenue fail");

        SQLINTEGER n = topN;
        SQLCHAR start[32], end[32];
        SQLLEN nInd, startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindInt(stmt.get(), 1, n, nInd);
        bindString(stmt.get(), 2, start, sizeof(start), startInd);
        bindString(stmt.get(), 3, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute top foods by revenue fail");

        SQLINTEGER foodId, totalQuantity;
        SQLCHAR foodName[256];
        SQLDOUBLE totalRevenue;
        SQLLEN foodIdLen, foodNameLen, totalQuantityLen, totalRevenueLen;

        bindTopFoodColumns(stmt.get(), foodId, foodIdLen, foodName, sizeof(foodName), foodNameLen,
                           totalQuantity, totalQuantityLen, totalRevenue, totalRevenueLen);

        int fetchCount = 0;
        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            TopFood item;
            item.foodId = foodId;
            item.foodName = getString(foodName, foodNameLen);
            item.totalQuantity = totalQuantity;
            item.totalRevenue = totalRevenue;
            items.push_back(item);
            fetchCount++;
        }
    } catch (const exception &e) {
        cout << "getTopFoodsByRevenue error: " << e.what() << endl;
    }

    return items;
}

vector<TransactionHistory> StatisticsDAL::getTransactionHistory(const string &startDate,
                                                                const string &endDate) {
    vector<TransactionHistory> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT b.bill_id, b.table_id, b.total_price, b.paid_date, "
            "       COUNT(bi.bill_item_id) AS itemCount "
            "FROM bills b "
            "LEFT JOIN bill_items bi ON b.bill_id = bi.bill_id "
            "WHERE b.paid_date IS NOT NULL AND b.paid_date BETWEEN ? AND ? "
            "GROUP BY b.bill_id, b.table_id, b.total_price, b.paid_date "
            "ORDER BY b.paid_date DESC";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare transaction history fail");

        SQLCHAR start[32], end[32];
        SQLLEN startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindString(stmt.get(), 1, start, sizeof(start), startInd);
        bindString(stmt.get(), 2, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute transaction history fail");

        SQLINTEGER billId, tableId, itemCount;
        SQLDOUBLE totalPrice;
        SQLCHAR paidDate[32];
        SQLLEN billIdLen, tableIdLen, totalPriceLen, paidDateLen, itemCountLen;

        bindTransactionHistoryColumns(stmt.get(), billId, billIdLen, tableId, tableIdLen,
                                      totalPrice, totalPriceLen, paidDate, sizeof(paidDate), paidDateLen,
                                      itemCount, itemCountLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            TransactionHistory item;
            item.billId = billId;
            item.tableId = tableId;
            item.totalPrice = totalPrice;
            item.paidDate = getString(paidDate, paidDateLen);
            item.itemCount = itemCount;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getTransactionHistory error: " << e.what() << endl;
    }

    return items;
}

vector<TransactionHistory> StatisticsDAL::getTransactionHistoryByTable(int tableId,
                                                                       const string &startDate,
                                                                       const string &endDate) {
    vector<TransactionHistory> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT b.bill_id, b.table_id, b.total_price, b.paid_date, "
            "       COUNT(bi.bill_item_id) AS itemCount "
            "FROM bills b "
            "LEFT JOIN bill_items bi ON b.bill_id = bi.bill_id "
            "WHERE b.paid_date IS NOT NULL AND b.table_id = ? AND b.paid_date BETWEEN ? AND ? "
            "GROUP BY b.bill_id, b.table_id, b.total_price, b.paid_date "
            "ORDER BY b.paid_date DESC";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare transaction history by table fail");

        SQLINTEGER tblId = tableId;
        SQLCHAR start[32], end[32];
        SQLLEN tblIdInd, startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindInt(stmt.get(), 1, tblId, tblIdInd);
        bindString(stmt.get(), 2, start, sizeof(start), startInd);
        bindString(stmt.get(), 3, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute transaction history by table fail");

        SQLINTEGER billId, tId, itemCount;
        SQLDOUBLE totalPrice;
        SQLCHAR paidDate[32];
        SQLLEN billIdLen, tIdLen, totalPriceLen, paidDateLen, itemCountLen;

        bindTransactionHistoryColumns(stmt.get(), billId, billIdLen, tId, tIdLen,
                                      totalPrice, totalPriceLen, paidDate, sizeof(paidDate), paidDateLen,
                                      itemCount, itemCountLen);

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            TransactionHistory item;
            item.billId = billId;
            item.tableId = tId;
            item.totalPrice = totalPrice;
            item.paidDate = getString(paidDate, paidDateLen);
            item.itemCount = itemCount;
            items.push_back(item);
        }
    } catch (const exception &e) {
        cout << "getTransactionHistoryByTable error: " << e.what() << endl;
    }

    return items;
}

bool StatisticsDAL::getTableStatistics(int tableId, const string &startDate, const string &endDate,
                                       int &billCount, double &totalRevenue) {
    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        const char *q =
            "SELECT COUNT(*) AS billCount, "
            "       COALESCE(SUM(total_price), 0) AS totalRevenue "
            "FROM bills "
            "WHERE paid_date IS NOT NULL AND table_id = ? AND paid_date BETWEEN ? AND ?";

        if (!prepareQuery(q, stmt.get()))
            stmt.throwStmtErr("prepare table statistics fail");

        SQLINTEGER tblId = tableId;
        SQLCHAR start[32], end[32];
        SQLLEN tblIdInd, startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindInt(stmt.get(), 1, tblId, tblIdInd);
        bindString(stmt.get(), 2, start, sizeof(start), startInd);
        bindString(stmt.get(), 3, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute table statistics fail");

        SQLINTEGER bc;
        SQLDOUBLE tr;
        SQLLEN bcLen, trLen;

        bindColumnInt(stmt.get(), 1, bc, bcLen);
        bindColumnDouble(stmt.get(), 2, tr, trLen);

        if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            billCount = bc;
            totalRevenue = tr;
            return true;
        }
    } catch (const exception &e) {
        cout << "getTableStatistics error: " << e.what() << endl;
    }

    return false;
}
