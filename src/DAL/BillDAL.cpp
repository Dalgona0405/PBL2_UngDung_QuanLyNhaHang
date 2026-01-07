#include "BillDAL.h"
#include <iostream>
using namespace std;

bool BillDAL::getOpenByTableId(int table_id, Bill &out) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q =
        "SELECT TOP 1 bill_id, table_id, total_price, paid_date "
        "FROM bills WHERE table_id=? AND paid_date IS NULL "
        "ORDER BY bill_id DESC";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare get open bill fail");

    SQLINTEGER t = table_id;
    SQLLEN tInd;
    bindInt(stmt.get(), 1, t, tInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute get open bill fail");

    bindBillColumns(stmt.get());

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        out = fetchBill();
        return true;
    }
    return false;
}

int BillDAL::createForTable(int tableId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "INSERT INTO bills (table_id, total_price, paid_date) OUTPUT INSERTED.bill_id VALUES (?, 0, NULL)";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare insert bill fail");

    SQLINTEGER t = tableId;
    SQLLEN tInd;
    bindInt(stmt.get(), 1, t, tInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute insert bill fail");

    SQLINTEGER newBillId = -1;
    SQLLEN billIdLen;
    bindColumnInt(stmt.get(), 1, newBillId, billIdLen);

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        return newBillId;
    }
    return -1;
}

bool BillDAL::deleteBill(int billId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "DELETE FROM bills WHERE bill_id=?";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare delete bill fail");

    SQLINTEGER id = billId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute delete bill fail");

    return true;
}

bool BillDAL::recalcTotal(int billId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q =
        "UPDATE bills SET total_price = "
        "  (SELECT COALESCE(SUM(sub_total),0) FROM bill_items WHERE bill_id=?) "
        "WHERE bill_id=?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare recalc fail");

    SQLINTEGER id = billId;
    SQLLEN idInd1, idInd2;

    bindInt(stmt.get(), 1, id, idInd1);
    bindInt(stmt.get(), 2, id, idInd2);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute recalc fail");

    return true;
}

bool BillDAL::getFoodPrice(int foodId, double &price) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "SELECT price FROM foods WHERE food_id=?";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare get food price fail");

    SQLINTEGER id = foodId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute get food price fail");

    double p = 0;
    SQLLEN pLen;
    bindColumnDouble(stmt.get(), 1, p, pLen);

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        price = p;
        return true;
    }
    return false;
}

bool BillDAL::addItem(int billId, int foodId, int quantity, const string &desc, int &outNewId) {
    double price = 0;
    if (!getFoodPrice(foodId, price))
        throw runtime_error("food not found");

    double sub = price * (quantity <= 0 ? 1 : quantity);

    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q =
        "INSERT INTO bill_items (bill_id, food_id, quantity, description, sub_total) "
        "OUTPUT INSERTED.bill_item_id "
        "VALUES (?, ?, ?, ?, ?)";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare insert item fail");

    SQLINTEGER b = billId, f = foodId, qtt = quantity;
    SQLDOUBLE subtotal = sub;
    SQLCHAR d[260];
    SQLLEN bL, fL, qL, dL, sL;

    safeCopyString(d, desc, sizeof(d));
    bindInt(stmt.get(), 1, b, bL);
    bindInt(stmt.get(), 2, f, fL);
    bindInt(stmt.get(), 3, qtt, qL);
    bindString(stmt.get(), 4, d, sizeof(d), dL);
    bindDouble(stmt.get(), 5, subtotal, sL);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute insert item fail");

    SQLINTEGER newId;
    SQLLEN idLen;
    bindColumnInt(stmt.get(), 1, newId, idLen);

    if (SQLFetch(stmt.get()) != SQL_SUCCESS)
        return false;

    outNewId = (int)newId;
    return true;
}

bool BillDAL::updateItem(int billItemId, int quantity, const string &desc) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt0(conn);

    const char *q0 = "SELECT food_id FROM bill_items WHERE bill_item_id=?";

    if (!prepareQuery(q0, stmt0.get()))
        stmt0.throwStmtErr("prepare get item fail");

    SQLINTEGER iid = billItemId;
    SQLLEN iidL;
    bindInt(stmt0.get(), 1, iid, iidL);

    if (!executeStatement(stmt0.get()))
        stmt0.throwStmtErr("execute get item fail");

    SQLINTEGER f = 0;
    SQLLEN fL;
    bindColumnInt(stmt0.get(), 1, f, fL);

    if (SQLFetch(stmt0.get()) != SQL_SUCCESS)
        return false;

    double price = 0;
    if (!getFoodPrice((int)f, price))
        throw runtime_error("food not found");
    double sub = price * (quantity <= 0 ? 1 : quantity);

    RAII_StatementHandle stmt(conn);
    const char *q =
        "UPDATE bill_items SET quantity=?, description=?, sub_total=? "
        "WHERE bill_item_id=?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare update item fail");

    SQLINTEGER qtt = quantity;
    SQLDOUBLE subtotal = sub;
    SQLCHAR d[260];
    SQLLEN qInd, dInd, sInd, idInd;

    safeCopyString(d, desc, sizeof(d));

    bindInt(stmt.get(), 1, qtt, qInd);
    bindString(stmt.get(), 2, d, sizeof(d), dInd);
    bindDouble(stmt.get(), 3, subtotal, sInd);
    bindInt(stmt.get(), 4, iid, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute update item fail");

    return true;
}

bool BillDAL::removeItem(int billItemId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "DELETE FROM bill_items WHERE bill_item_id=?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare delete item fail");

    SQLINTEGER id = billItemId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute delete item fail");

    return true;
}

bool BillDAL::getItemContext(int billItemId, int &billId, int &tableId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q =
        "SELECT bi.bill_id, b.table_id "
        "FROM bill_items bi INNER JOIN bills b ON bi.bill_id = b.bill_id "
        "WHERE bi.bill_item_id=?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare context fail");

    SQLINTEGER iid = billItemId;
    SQLLEN iidInd;
    bindInt(stmt.get(), 1, iid, iidInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute context fail");

    SQLINTEGER b = 0, t = 0;
    SQLLEN bInd, tInd;

    bindColumnInt(stmt.get(), 1, b, bInd);
    bindColumnInt(stmt.get(), 2, t, tInd);

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        billId = (int)b;
        tableId = (int)t;
        return true;
    }

    return false;
}

bool BillDAL::listItemsByBill(int billId, vector<BillItem> &out) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "SELECT bill_item_id, bill_id, food_id, quantity, description, sub_total FROM bill_items WHERE bill_id=? ORDER BY bill_item_id";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare list items fail");

    SQLINTEGER id = billId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute list items fail");

    SQLINTEGER bi, b, f, qty;
    SQLDOUBLE sub;
    SQLCHAR desc[260];
    SQLLEN biLen, bLen, fLen, qtyLen, dLen, subLen;

    bindColumnInt(stmt.get(), 1, bi, biLen);
    bindColumnInt(stmt.get(), 2, b, bLen);
    bindColumnInt(stmt.get(), 3, f, fLen);
    bindColumnInt(stmt.get(), 4, qty, qtyLen);
    bindColumnString(stmt.get(), 5, desc, sizeof(desc), dLen);
    bindColumnDouble(stmt.get(), 6, sub, subLen);

    while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        BillItem it;
        it.id = bi;
        it.bill_id = b;
        it.food_id = f;
        it.quantity = qty;
        it.description = getString(desc, dLen);
        it.sub_total = sub;
        out.push_back(it);
    }
    return true;
}

bool BillDAL::closeBill(int billId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "UPDATE bills SET paid_date = GETDATE() WHERE bill_id=? AND paid_date IS NULL";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare close fail");

    SQLINTEGER id = billId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute close fail");

    return true;
}

int BillDAL::countItems(int billId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "SELECT COUNT(*) FROM bill_items WHERE bill_id=?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare count items fail");

    SQLINTEGER id = billId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute count items fail");

    SQLINTEGER c = 0;
    SQLLEN cLen;
    bindColumnInt(stmt.get(), 1, c, cLen);

    int result = 0;
    if (SQLFetch(stmt.get()) == SQL_SUCCESS)
        result = (int)c;
    return result;
}

vector<Bill> BillDAL::getAll() {
    vector<Bill> items;
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    string query = getBaseQuery() + " ORDER BY bill_id";

    if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
        stmt.throwStmtErr("select bills fail");

    bindBillColumns(stmt.get());

    while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        items.push_back(fetchBill());
    }

    return items;
}

bool BillDAL::getById(int idQuery, Bill &out) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    string query = getBaseQuery() + " WHERE bill_id = ?";

    if (!prepareQuery(query.c_str(), stmt.get()))
        stmt.throwStmtErr("prepare get fail");

    SQLINTEGER id = idQuery;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute get fail");

    bindBillColumns(stmt.get());

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        out = fetchBill();
        return true;
    }
    return false;
}

vector<Bill> BillDAL::searchByTableNumber(int tableNumber) {
    vector<Bill> items;

    if (tableNumber <= 0) {
        return getAll();
    }

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE table_id = ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by table fail");

        SQLINTEGER tblNum = tableNumber;
        SQLLEN tblInd;
        bindInt(stmt.get(), 1, tblNum, tblInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by table fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "searchByTableNumber error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::searchByDateRange(const string &startDate, const string &endDate) {
    vector<Bill> items;

    if (startDate.empty() || endDate.empty()) {
        return getAll();
    }

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE paid_date BETWEEN ? AND ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by date fail");

        SQLCHAR start[32], end[32];
        SQLLEN startInd, endInd;

        safeCopyString(start, startDate, sizeof(start));
        safeCopyString(end, endDate, sizeof(end));

        bindString(stmt.get(), 1, start, sizeof(start), startInd);
        bindString(stmt.get(), 2, end, sizeof(end), endInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by date fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "searchByDateRange error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::searchByStatus(bool isPaid) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + (isPaid ? " WHERE paid_date IS NOT NULL" : " WHERE paid_date IS NULL");

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute search by status fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "searchByStatus error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::searchByPriceRange(double minPrice, double maxPrice) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE total_price BETWEEN ? AND ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by price fail");

        SQLDOUBLE minP = minPrice, maxP = maxPrice;
        SQLLEN minInd, maxInd;

        bindDouble(stmt.get(), 1, minP, minInd);
        bindDouble(stmt.get(), 2, maxP, maxInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by price fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "searchByPriceRange error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::getAllSortedByBillId(bool ascending) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("bill_id", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by bill_id fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByBillId error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::getAllSortedByTableNumber(bool ascending) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("table_id", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by table fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByTableNumber error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::getAllSortedByTotalPrice(bool ascending) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("total_price", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by price fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByTotalPrice error: " << e.what() << endl;
    }

    return items;
}

vector<Bill> BillDAL::getAllSortedByPaidDate(bool ascending) {
    vector<Bill> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("paid_date", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by date fail");

        bindBillColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchBill());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByPaidDate error: " << e.what() << endl;
    }

    return items;
}