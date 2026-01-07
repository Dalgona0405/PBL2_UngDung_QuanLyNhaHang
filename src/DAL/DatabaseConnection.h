#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>

class DatabaseConnection {
	private:
		SQLHANDLE env;
		SQLHANDLE conn;

		DatabaseConnection();
		DatabaseConnection(const DatabaseConnection&);
		DatabaseConnection& operator=(const DatabaseConnection&);

	public:
		~DatabaseConnection();
		static DatabaseConnection& instance();
		SQLHANDLE getConnection();
};

#endif

