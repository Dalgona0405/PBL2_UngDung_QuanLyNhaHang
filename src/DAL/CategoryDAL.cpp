#include "CategoryDAL.h"
#include <iostream>
using namespace std;

vector<Category> CategoryDAL::getAll() {
	vector<Category> items;

	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string query = getBaseQuery() + " ORDER BY c.category_id";

	if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
		stmt.throwStmtErr("select categories fail");

	bindCategoryColumns(stmt.get());

	while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		items.push_back(fetchCategory());
	}

	return items;
}

bool CategoryDAL::getById(int idQuery, Category& out) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string q = getBaseQuery() + " WHERE c.category_id = ?";

	if (!prepareQuery(q.c_str(), stmt.get()))
		stmt.throwStmtErr("prepare get fail");

	SQLINTEGER id = idQuery;
	SQLLEN idInd;
	bindInt(stmt.get(), 1, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute get fail");

	bindCategoryColumns(stmt.get());

	if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		out = fetchCategory();
		return true;
	}
	return false;
}

bool CategoryDAL::insert(const Category& category) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "INSERT INTO categories (category_name) VALUES (?)";
	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare insert fail");

	SQLCHAR name[256];
	SQLLEN nameInd;

	safeCopyString(name, category.name, sizeof(name));
	bindString(stmt.get(), 1, name, sizeof(name), nameInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute insert fail");

	return true;
}

bool CategoryDAL::update(const Category& category) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "UPDATE categories SET category_name = ? WHERE category_id = ?";

	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare update fail");

	SQLCHAR name[256];
	SQLINTEGER id = category.id;
	SQLLEN nameInd, idInd;

	safeCopyString(name, category.name, sizeof(name));
	bindString(stmt.get(), 1, name, sizeof(name), nameInd);
	bindInt(stmt.get(), 2, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute update fail");

	return true;
}

bool CategoryDAL::remove(int idQuery) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "DELETE FROM categories WHERE category_id = ?";
	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare delete fail");

	SQLINTEGER id = idQuery;
	SQLLEN idInd;
	bindInt(stmt.get(), 1, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute delete fail");

	return true;
}

vector<Category> CategoryDAL::searchByName(const string& keyword) {
	vector<Category> items;

	if (keyword.empty()) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE c.category_name LIKE ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by name fail");

		string pattern = buildLikePattern(keyword);
		SQLCHAR patternBuf[256];
		SQLLEN patternInd;
		safeCopyString(patternBuf, pattern, sizeof(patternBuf));
		bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by name fail");

		bindCategoryColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchCategory());
		}
	} catch (const exception& e) {
		cout << "searchByName error: " << e.what() << endl;
		return vector<Category>();
	}

	return items;
}

vector<Category> CategoryDAL::getAllSortedByName(bool ascending) {
	vector<Category> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("c.category_name", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by name fail");

		bindCategoryColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchCategory());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByName error: " << e.what() << endl;
		return vector<Category>();
	}

	return items;
}