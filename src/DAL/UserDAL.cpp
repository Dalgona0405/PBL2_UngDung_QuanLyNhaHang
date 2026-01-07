#include "UserDAL.h"
#include <iostream>
using namespace std;

bool UserDAL::login(const string& userName, const string& password, User& out) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q =
	    "SELECT user_id, user_name, password, full_name, phone_number, "
	    "birth, gender_id, role_id "
	    "FROM users WHERE user_name = ? AND password = ?";

	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare login fail");

	SQLCHAR uname[50], pwd[50];
	SQLLEN unameInd, pwdInd;

	safeCopyString(uname, userName, sizeof(uname));
	safeCopyString(pwd, password, sizeof(pwd));

	bindString(stmt.get(), 1, uname, sizeof(uname), unameInd);
	bindString(stmt.get(), 2, pwd, sizeof(pwd), pwdInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute login fail");

	bindUserColumns(stmt.get());

	if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		out = fetchUser();
		return true;
	}
	return false;
}

vector<User> UserDAL::getAll() {
	vector<User> items;

	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string query = getBaseQuery() + " WHERE role_id = 0 ORDER BY user_id";

	if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
		stmt.throwStmtErr("select users fail");

	bindUserColumns(stmt.get());

	while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		items.push_back(fetchUser());
	}

	return items;
}

bool UserDAL::getById(int idQuery, User& out) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	string q = getBaseQuery() + " WHERE user_id = ?";

	if (!prepareQuery(q.c_str(), stmt.get()))
		stmt.throwStmtErr("prepare get fail");

	SQLINTEGER id = idQuery;
	SQLLEN idInd;
	bindInt(stmt.get(), 1, id, idInd);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute get fail");

	bindUserColumns(stmt.get());

	if (SQLFetch(stmt.get()) == SQL_SUCCESS) {
		out = fetchUser();
		return true;
	}
	return false;
}

bool UserDAL::insert(const User& u) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q =
	    "INSERT INTO users (user_name, password, full_name, phone_number, birth, gender_id, role_id) "
	    "VALUES (?, ?, ?, ?, ?, ?, ?)";

	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare insert user fail");

	SQLCHAR un[64], pw[64], fn[128], ph[32], br[32];
	safeCopyString(un, u.userName, sizeof(un));
	safeCopyString(pw, u.password, sizeof(pw));
	safeCopyString(fn, u.fullName, sizeof(fn));
	safeCopyString(ph, u.phoneNumber, sizeof(ph));
	safeCopyString(br, u.birth, sizeof(br));

	SQLINTEGER genderId = u.genderId, roleId = u.roleId;
	SQLLEN unLen, pwLen, fnLen, phLen, brLen, gLen, rLen;

	bindString(stmt.get(), 1, un, sizeof(un), unLen);
	bindString(stmt.get(), 2, pw, sizeof(pw), pwLen);
	bindString(stmt.get(), 3, fn, sizeof(fn), fnLen);
	bindString(stmt.get(), 4, ph, sizeof(ph), phLen);
	bindString(stmt.get(), 5, br, sizeof(br), brLen);
	bindInt(stmt.get(), 6, genderId, gLen);
	bindInt(stmt.get(), 7, roleId, rLen);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute insert user fail");

	return true;
}

bool UserDAL::update(const User& u) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q =
	    "UPDATE users SET user_name=?, password=?, full_name=?, phone_number=?, birth=?, gender_id=?, role_id=? "
	    "WHERE user_id=?";

	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare update user fail");

	SQLCHAR un[64], pw[64], fn[128], ph[32], br[32];
	safeCopyString(un, u.userName, sizeof(un));
	safeCopyString(pw, u.password, sizeof(pw));
	safeCopyString(fn, u.fullName, sizeof(fn));
	safeCopyString(ph, u.phoneNumber, sizeof(ph));
	safeCopyString(br, u.birth, sizeof(br));

	SQLINTEGER genderId = u.genderId, roleId = u.roleId, id = u.id;
	SQLLEN unLen, pwLen, fnLen, phLen, brLen, gLen, rLen, idLen;

	bindString(stmt.get(), 1, un, sizeof(un), unLen);
	bindString(stmt.get(), 2, pw, sizeof(pw), pwLen);
	bindString(stmt.get(), 3, fn, sizeof(fn), fnLen);
	bindString(stmt.get(), 4, ph, sizeof(ph), phLen);
	bindString(stmt.get(), 5, br, sizeof(br), brLen);
	bindInt(stmt.get(), 6, genderId, gLen);
	bindInt(stmt.get(), 7, roleId, rLen);
	bindInt(stmt.get(), 8, id, idLen);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute update user fail");

	return true;
}

bool UserDAL::remove(int idQuery) {
	SQLHANDLE conn = getConnection();
	RAII_StatementHandle stmt(conn);

	const char* q = "DELETE FROM users WHERE user_id = ?";
	if (!prepareQuery(q, stmt.get()))
		stmt.throwStmtErr("prepare delete user fail");

	SQLINTEGER id = idQuery;
	SQLLEN idLen;
	bindInt(stmt.get(), 1, id, idLen);

	if (!executeStatement(stmt.get()))
		stmt.throwStmtErr("execute delete user fail");

	return true;
}

vector<User> UserDAL::searchByUserName(const string& keyword) {
	vector<User> items;

	if (keyword.empty()) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE user_name LIKE ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by username fail");

		string pattern = buildLikePattern(keyword);
		SQLCHAR patternBuf[128];
		SQLLEN patternInd;
		safeCopyString(patternBuf, pattern, sizeof(patternBuf));
		bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by username fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByUserName error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::searchByFullName(const string& keyword) {
	vector<User> items;

	if (keyword.empty()) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE full_name LIKE ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by fullname fail");

		string pattern = buildLikePattern(keyword);
		SQLCHAR patternBuf[256];
		SQLLEN patternInd;
		safeCopyString(patternBuf, pattern, sizeof(patternBuf));
		bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by fullname fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByFullName error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::searchByPhone(const string& keyword) {
	vector<User> items;

	if (keyword.empty()) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE phone_number LIKE ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by phone fail");

		string pattern = buildLikePattern(keyword);
		SQLCHAR patternBuf[64];
		SQLLEN patternInd;
		safeCopyString(patternBuf, pattern, sizeof(patternBuf));
		bindString(stmt.get(), 1, patternBuf, sizeof(patternBuf), patternInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by phone fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByPhone error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::searchByGenderId(int genderId) {
	vector<User> items;

	if (genderId != 0 && genderId != 1) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE gender_id = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by gender fail");

		SQLINTEGER gId = genderId;
		SQLLEN gInd;
		bindInt(stmt.get(), 1, gId, gInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by gender fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByGenderId error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::searchByRoleId(int roleId) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE role_id = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by role fail");

		SQLINTEGER rId = roleId;
		SQLLEN rInd;
		bindInt(stmt.get(), 1, rId, rInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by role fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByRoleId error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::searchByBirthYear(int year) {
	vector<User> items;

	if (year <= 0) {
		return getAll();
	}

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + " WHERE YEAR(birth) = ?";

		if (!prepareQuery(query.c_str(), stmt.get()))
			stmt.throwStmtErr("prepare search by birth year fail");

		SQLINTEGER yr = year;
		SQLLEN yrInd;
		bindInt(stmt.get(), 1, yr, yrInd);

		if (!executeStatement(stmt.get()))
			stmt.throwStmtErr("execute search by birth year fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "searchByBirthYear error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::getAllSortedByUserName(bool ascending) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("user_name", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by username fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByUserName error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::getAllSortedByFullName(bool ascending) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("full_name", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by fullname fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByFullName error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::getAllSortedByGender(bool ascending) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("gender_id", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by gender fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByGender error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::getAllSortedByBirth(bool ascending) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("birth", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by birth fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByBirth error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}

vector<User> UserDAL::getAllSortedByRole(bool ascending) {
	vector<User> items;

	try {
		SQLHANDLE conn = getConnection();
		RAII_StatementHandle stmt(conn);

		string query = getBaseQuery() + buildOrderByClause("role_id", ascending);

		if (SQLExecDirect(stmt.get(), (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS)
			stmt.throwStmtErr("execute sorted by role fail");

		bindUserColumns(stmt.get());

		while (SQLFetch(stmt.get()) == SQL_SUCCESS) {
			items.push_back(fetchUser());
		}
	} catch (const exception& e) {
		cout << "getAllSortedByRole error: " << e.what() << endl;
		return vector<User>();
	}

	return items;
}