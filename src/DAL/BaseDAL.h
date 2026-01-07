#ifndef BASE_DAL_H
#define BASE_DAL_H
#include "DatabaseConnection.h"
#include "RAII_StatementHandle.h"
#include <string>
using namespace std;

class BaseDAL {
	protected:
		SQLHANDLE getConnection();
		bool prepareQuery(const char*, SQLHANDLE);
		bool executeStatement(SQLHANDLE);

		void bindInt(SQLHANDLE, int, SQLINTEGER&, SQLLEN&);
		void bindDouble(SQLHANDLE, int, SQLDOUBLE&, SQLLEN&);
		void bindString(SQLHANDLE, int, SQLCHAR*, size_t, SQLLEN&);

		void bindColumnInt(SQLHANDLE, int, SQLINTEGER&, SQLLEN&);
		void bindColumnDouble(SQLHANDLE, int, SQLDOUBLE&, SQLLEN&);
		void bindColumnString(SQLHANDLE, int, SQLCHAR*, size_t, SQLLEN&);

		void safeCopyString(SQLCHAR*, const string&, size_t);
		bool isNull(SQLLEN);
		string getString(SQLCHAR*, SQLLEN);
		string buildLikePattern(const string&);
		string buildOrderByClause(const string&, bool);
};

#endif