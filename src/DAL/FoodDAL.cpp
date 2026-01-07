#include "FoodDAL.h"
#include <iostream>
using namespace std;

vector<Food> FoodDAL::getAll() {
    vector<Food> items;
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    string query = getBaseQuery() + " ORDER BY f.food_id";
    if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
        stmt.throwStmtErr("select foods fail");
    bindFoodColumns(stmt.get());
    while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        items.push_back(fetchFood());
    }
    return items;
}

bool FoodDAL::getById(int idQuery, Food &out) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    string q = getBaseQuery() + " WHERE f.food_id = ?";

    if (!prepareQuery(q.c_str(), stmt.get()))
        stmt.throwStmtErr("prepare get fail");

    SQLINTEGER id = idQuery;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute get fail");

    bindFoodColumns(stmt.get());

    if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
        out = fetchFood();
        return true;
    }
    return false;
}

bool FoodDAL::insert(const Food &food) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "INSERT INTO foods (food_name, category_id, price) VALUES (?, ?, ?)";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare insert fail");

    SQLCHAR nameBuffer[256];
    SQLINTEGER category_id = food.categoryId;
    SQLDOUBLE priceValue = food.price;
    SQLLEN nameInd, categoryInd, priceInd;

    safeCopyString(nameBuffer, food.name, sizeof(nameBuffer));
    bindString(stmt.get(), 1, nameBuffer, sizeof(nameBuffer), nameInd);
    bindInt(stmt.get(), 2, category_id, categoryInd);
    bindDouble(stmt.get(), 3, priceValue, priceInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute insert fail");

    return true;
}

bool FoodDAL::update(const Food &food) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "UPDATE foods SET food_name = ?, category_id = ?, price = ? WHERE food_id = ?";

    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare update fail");

    SQLCHAR nameBuffer[256];
    SQLINTEGER category_id = food.categoryId, id = food.id;
    SQLDOUBLE priceValue = food.price;
    SQLLEN nameInd, categoryInd, priceInd, idInd;

    safeCopyString(nameBuffer, food.name, sizeof(nameBuffer));
    bindString(stmt.get(), 1, nameBuffer, sizeof(nameBuffer), nameInd);
    bindInt(stmt.get(), 2, category_id, categoryInd);
    bindDouble(stmt.get(), 3, priceValue, priceInd);
    bindInt(stmt.get(), 4, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute update fail");

    return true;
}

bool FoodDAL::remove(int idQuery) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "DELETE FROM foods WHERE food_id = ?";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare delete fail");

    SQLINTEGER id = idQuery;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute delete fail");

    return true;
}

bool FoodDAL::setCategoryIdToNull(int categoryId) {
    SQLHANDLE conn = getConnection();
    RAII_StatementHandle stmt(conn);

    const char *q = "UPDATE foods SET category_id = NULL WHERE category_id = ?";
    if (!prepareQuery(q, stmt.get()))
        stmt.throwStmtErr("prepare set category_id null fail");

    SQLINTEGER id = categoryId;
    SQLLEN idInd;
    bindInt(stmt.get(), 1, id, idInd);

    if (!executeStatement(stmt.get()))
        stmt.throwStmtErr("execute set null fail");

    return true;
}

vector<Food> FoodDAL::searchByName(const string &keyword) {
    vector<Food> items;

    if (keyword.empty()) {
        return getAll();
    }

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE f.food_name LIKE ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by name fail");

        string pattern = buildLikePattern(keyword);
        SQLCHAR patternBuf[256];
        SQLLEN patternInd;
        safeCopyString(patternBuf, pattern, sizeof(patternBuf));
        bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by name fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "searchByName error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}

vector<Food> FoodDAL::searchByCategory(const string &categoryName) {
    vector<Food> items;

    if (categoryName.empty()) {
        return getAll();
    }

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE c.category_name LIKE ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by category name fail");

        string pattern = buildLikePattern(categoryName);
        SQLCHAR patternBuf[256];
        SQLLEN patternInd;
        safeCopyString(patternBuf, pattern, sizeof(patternBuf));
        bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by category name fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "searchByCategory error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}

vector<Food> FoodDAL::searchByPriceRange(double minPrice, double maxPrice) {
    vector<Food> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + " WHERE f.price BETWEEN ? AND ?";

        if (!prepareQuery(query.c_str(), stmt.get()))
            stmt.throwStmtErr("prepare search by price fail");

        SQLDOUBLE minP = minPrice, maxP = maxPrice;
        SQLLEN minInd, maxInd;
        bindDouble(stmt.get(), 1, minP, minInd);
        bindDouble(stmt.get(), 2, maxP, maxInd);

        if (!executeStatement(stmt.get()))
            stmt.throwStmtErr("execute search by price fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "searchByPriceRange error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}

vector<Food> FoodDAL::getAllSortedByName(bool ascending) {
    vector<Food> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("f.food_name", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by name fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByName error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}

vector<Food> FoodDAL::getAllSortedByPrice(bool ascending) {
    vector<Food> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("f.price", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by price fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByPrice error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}

vector<Food> FoodDAL::getAllSortedByCategory(bool ascending) {
    vector<Food> items;

    try {
        SQLHANDLE conn = getConnection();
        RAII_StatementHandle stmt(conn);

        string query = getBaseQuery() + buildOrderByClause("c.category_name", ascending);

        if (SQLExecDirect(stmt.get(), (SQLCHAR *)query.c_str(), SQL_NTS) != SQL_SUCCESS)
            stmt.throwStmtErr("execute sorted by category fail");

        bindFoodColumns(stmt.get());

        while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
            items.push_back(fetchFood());
        }
    } catch (const exception &e) {
        cout << "getAllSortedByCategory error: " << e.what() << endl;
        return vector<Food>();
    }
    return items;
}