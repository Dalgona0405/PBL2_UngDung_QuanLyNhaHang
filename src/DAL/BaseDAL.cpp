#include "BaseDAL.h"
#include <vector>

SQLHANDLE BaseDAL::getConnection() {
    return DatabaseConnection::instance().getConnection();
}

bool BaseDAL::prepareQuery(const char *query, SQLHANDLE stmt) {
    return SQLPrepare(stmt, (SQLCHAR *)query, SQL_NTS) == SQL_SUCCESS;
}

bool BaseDAL::executeStatement(SQLHANDLE stmt) {
    SQLRETURN rc = SQLExecute(stmt);
    return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA);
}

void BaseDAL::bindInt(SQLHANDLE stmt, int position, SQLINTEGER &value, SQLLEN &indicator) {
    indicator = 0;
    SQLBindParameter(
        stmt,
        position,
        SQL_PARAM_INPUT,
        SQL_C_LONG,
        SQL_INTEGER,
        0,
        0,
        &value,
        0,
        &indicator);
}

void BaseDAL::bindDouble(SQLHANDLE stmt, int position, SQLDOUBLE &value, SQLLEN &indicator) {
    indicator = 0;
    SQLBindParameter(
        stmt,
        position,
        SQL_PARAM_INPUT,
        SQL_C_DOUBLE,
        SQL_DOUBLE,
        0,
        0,
        &value,
        0,
        &indicator);
}

void BaseDAL::bindString(SQLHANDLE stmt, int position, SQLCHAR *buffer, size_t bufferSize, SQLLEN &indicator) {
    indicator = SQL_NTS;
    SQLBindParameter(
        stmt,
        position,
        SQL_PARAM_INPUT,
        SQL_C_CHAR,
        SQL_VARCHAR,
        bufferSize - 1,
        0,
        buffer,
        0,
        &indicator);
}

void BaseDAL::bindColumnInt(SQLHANDLE stmt, int column, SQLINTEGER &value, SQLLEN &indicator) {
    SQLBindCol(
        stmt,
        column,
        SQL_C_LONG,
        &value,
        0,
        &indicator);
}

void BaseDAL::bindColumnDouble(SQLHANDLE stmt, int column, SQLDOUBLE &value, SQLLEN &indicator) {
    SQLBindCol(
        stmt,
        column,
        SQL_C_DOUBLE,
        &value,
        0,
        &indicator);
}

void BaseDAL::bindColumnString(SQLHANDLE stmt, int column, SQLCHAR *buffer, size_t bufferSize, SQLLEN &indicator) {
    SQLBindCol(
        stmt,
        column,
        SQL_C_CHAR,
        buffer,
        bufferSize,
        &indicator);
}

void BaseDAL::safeCopyString(SQLCHAR *dest, const string &src, size_t destSize) {
    if (src.empty()) {
        dest[0] = '\0';
        return;
    }
    size_t len = min(src.length(), destSize - 1);
    memcpy(dest, src.c_str(), len);
    dest[len] = '\0';
}

bool BaseDAL::isNull(SQLLEN indicator) {
    return indicator == SQL_NULL_DATA;
}

string BaseDAL::getString(SQLCHAR *buffer, SQLLEN indicator) {
    if (isNull(indicator)) {
        return "";
    }
    return string((char *)buffer);
}

string BaseDAL::buildLikePattern(const string &keyword) {
    return "%" + keyword + "%";
}

string BaseDAL::buildOrderByClause(const string &column, bool ascending) {
    return " ORDER BY " + column + (ascending ? " ASC" : " DESC");
}