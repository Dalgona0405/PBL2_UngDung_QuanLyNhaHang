#include "TableDAL.h"
#include <iostream>
using namespace std;

vector<Table> TableDAL::getAll() {
	vector<Table> items;

	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string query = getBaseQuery() + " ORDER BY t.table_id";

	if(SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
		stmt.throwStmtErr("select tables fail");

	bindTableColumns(stmt.get());

	while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		items.push_back(fetchTable());
	}

	return items;
}

bool TableDAL::getById(int idQuery, Table& out) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string q = getBaseQuery() + " WHERE t.table_id = ?";

	if (!prepareQuery(q.c_str(), stmt.get()))
		stmt.throwStmtErr("prepare get fail");

	SQLINTEGER id = idQuery;
	SQLLEN idInd;
	bindInt(stmt.get(), 1, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute get fail");

	bindTableColumns(stmt.get());

	if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		out = fetchTable();
		return true;
	}
	return false;
}

bool TableDAL::insert(const Table& table) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "INSERT INTO tables (table_number, capacity, status_id) VALUES (?, ?, ?)";
	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare insert fail");

	SQLINTEGER number = table.number;
	SQLINTEGER capacity = table.capacity;
	SQLINTEGER status_id = table.status_id;
	SQLLEN numberInd, capacityInd, statusInd;

	bindInt(stmt.get(), 1, number, numberInd);
	bindInt(stmt.get(), 2, capacity, capacityInd);
	bindInt(stmt.get(), 3, status_id, statusInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute insert fail");

	return true;
}


bool TableDAL::update(const Table& table) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "UPDATE tables SET table_number=?, capacity=?, status_id=? WHERE table_id=?";

	if(!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare update fail");

	SQLINTEGER number = table.number;
	SQLINTEGER capacity = table.capacity;
	SQLINTEGER status_id = table.status_id;
	SQLINTEGER id = table.id;
	SQLLEN numberInd, capacityInd, statusInd, idInd;

	bindInt(stmt.get(), 1, number, numberInd);
	bindInt(stmt.get(), 2, capacity, capacityInd);
	bindInt(stmt.get(), 3, status_id, statusInd);
	bindInt(stmt.get(), 4, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute update fail");

	return true;
}

bool TableDAL::remove(int idQuery) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "DELETE FROM tables WHERE table_id=?";
	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare delete fail");

	SQLINTEGER id = idQuery;
	SQLLEN idInd;

	bindInt(stmt.get(), 1, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute delete fail");

	return true;
}

vector<Table> TableDAL::searchByNumber(int number) {
	vector<Table> items;

	if (number <= 0) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE t.table_number = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by number fail");

		SQLINTEGER tNumber = number;
		SQLLEN nInd;
		bindInt(stmt.get(), 1, tNumber, nInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by number fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "searchByNumber error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}

vector<Table> TableDAL::searchByCapacity(int capacity) {
	vector<Table> items;

	if (capacity <= 0) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE t.capacity = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by capacity fail");

		SQLINTEGER tCapacity = capacity;
		SQLLEN cInd;
		bindInt(stmt.get(), 1, tCapacity, cInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by capacity fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "searchByCapacity error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}

vector<Table> TableDAL::searchByStatus(int status) {
	vector<Table> items;

	if (status != 0 && status != 1) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE t.status_id = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by status fail");

		SQLINTEGER tStatus = status;
		SQLLEN sInd;
		bindInt(stmt.get(), 1, tStatus, sInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by status fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "searchByStatus error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}

vector<Table> TableDAL::getAllSortedByNumber(bool ascending) {
	vector<Table> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("t.table_number", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by number fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByNumber error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}

vector<Table> TableDAL::getAllSortedByCapacity(bool ascending) {
	vector<Table> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("t.capacity", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by capacity fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByCapacity error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}

vector<Table> TableDAL::getAllSortedByStatus(bool ascending) {
	vector<Table> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("t.status_id", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by status fail");

		bindTableColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchTable());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByStatus error: " << e.what() << endl;
		return vector<Table>();
	}

	return items;
}