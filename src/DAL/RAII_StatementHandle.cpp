#include "RAII_StatementHandle.h"
#include <string>
using namespace std;

void RAII_StatementHandle::throwErr(SQLHANDLE h, const char* msg) {
	SQLCHAR state[6], text[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER native;
	SQLSMALLINT len;
	SQLGetDiagRec(SQL_HANDLE_STMT, h, 1, state, &native, text, sizeof(text), &len);
	throw runtime_error(string(msg) + " - " + (char*)text);
}

RAII_StatementHandle::RAII_StatementHandle(SQLHANDLE conn) : stmt(SQL_NULL_HANDLE) {
	if (SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt) != SQL_SUCCESS) {
		throw runtime_error("RAII: SQLAllocHandle failed");
	}
}

RAII_StatementHandle::~RAII_StatementHandle() {
	if (stmt != SQL_NULL_HANDLE) {
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	}
}

void RAII_StatementHandle::throwStmtErr(const char* msg) {
	throwErr(this->stmt, msg);
}

SQLHANDLE RAII_StatementHandle::get() {
	return stmt;
}