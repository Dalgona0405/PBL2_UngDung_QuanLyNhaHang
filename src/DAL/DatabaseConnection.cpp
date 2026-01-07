#include "DatabaseConnection.h"
#include <iostream>
#include <string>

using namespace std;

DatabaseConnection &DatabaseConnection::instance() {
    static DatabaseConnection inst;
    return inst;
}

DatabaseConnection::DatabaseConnection() {
    SQLRETURN ret;

    // Allocate environment
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        throw runtime_error("Error: Failed to allocate environment handle.");
    }

    // Set ODBC version 3
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        throw runtime_error("Error: Failed to set ODBC version.");
    }

    // Allocate connection
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &conn);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        throw runtime_error("Error: Failed to allocate connection handle.");
    }

    SQLCHAR connStr[] = "{ODBC Driver 17 for SQL Server};SERVER=.\\SQLEXPRESS;DATABASE=RestaurentDB;Trusted_Connection=Yes;Encrypt=Yes;TrustServerCertificate=Yes;";
    SQLCHAR outStr[1024];
    SQLSMALLINT outStrLen;

    ret = SQLDriverConnect(conn, NULL, connStr, SQL_NTS, outStr, sizeof(outStr), &outStrLen, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        SQLGetDiagRec(SQL_HANDLE_DBC, conn, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen);
        string error = "Error: Connection failed - " + string((char *)msg);
        throw runtime_error(error);
    }

    SQLHANDLE stmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) == SQL_SUCCESS) {
        SQLCHAR *setQuery = (SQLCHAR *)"SET ARITHABORT ON";
        SQLExecDirect(stmt, setQuery, SQL_NTS);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

DatabaseConnection::~DatabaseConnection() {
    SQLDisconnect(conn);
    SQLFreeHandle(SQL_HANDLE_DBC, conn);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
}

SQLHANDLE DatabaseConnection::getConnection() {
    return conn;
}
