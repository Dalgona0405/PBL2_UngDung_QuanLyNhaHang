#ifndef RAII_STATEMENT_HANDLE_H
#define RAII_STATEMENT_HANDLE_H
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>

class RAII_StatementHandle {
	private:
		SQLHANDLE stmt;
		static void throwErr(SQLHANDLE h, const char* msg);

	public:
		RAII_StatementHandle(SQLHANDLE conn);
		~RAII_StatementHandle();
		SQLHANDLE get();
		void throwStmtErr(const char* msg);
		RAII_StatementHandle(const RAII_StatementHandle&) = delete;
		RAII_StatementHandle& operator=(const RAII_StatementHandle&) = delete;
};
#endif