#ifndef USER_DAL_H
#define USER_DAL_H
#include "BaseDAL.h"
#include "../Models/User.h"
#include <vector>
using namespace std;

class UserDAL : public BaseDAL {
	private:
		SQLINTEGER id, genderId, roleId;
		SQLCHAR userName[64], password[64], fullName[128], phone[32], birth[32];
		SQLLEN idLen, unLen, pwLen, fnLen, phLen, bLen, gLen, rLen;

		void bindUserColumns(SQLHANDLE stmt) {
			bindColumnInt(stmt, 1, id, idLen);
			bindColumnString(stmt, 2, userName, sizeof(userName), unLen);
			bindColumnString(stmt, 3, password, sizeof(password), pwLen);
			bindColumnString(stmt, 4, fullName, sizeof(fullName), fnLen);
			bindColumnString(stmt, 5, phone, sizeof(phone), phLen);
			bindColumnString(stmt, 6, birth, sizeof(birth), bLen);
			bindColumnInt(stmt, 7, genderId, gLen);
			bindColumnInt(stmt, 8, roleId, rLen);
		}

		User fetchUser() {
			User u;
			u.id = id;
			u.userName = getString(userName, unLen);
			u.password = getString(password, pwLen);
			u.fullName = getString(fullName, fnLen);
			u.phoneNumber = getString(phone, phLen);
			u.birth = getString(birth, bLen);
			u.genderId = genderId;
			u.roleId = roleId;
			return u;
		}

		string getBaseQuery() const {
			return "SELECT user_id, user_name, password, full_name, phone_number, birth, gender_id, role_id FROM users";
		}

	public:
		// Authentication
		bool login(const string& userName, const string& password, User& out);

		// CRUD Operations
		vector<User> getAll();
		bool getById(int id, User& out);
		bool insert(const User& u);
		bool update(const User& u);
		bool remove(int id);

		// Search Functions
		vector<User> searchByUserName(const string& keyword);
		vector<User> searchByFullName(const string& keyword);
		vector<User> searchByPhone(const string& keyword);
		vector<User> searchByGenderId(int genderId);
		vector<User> searchByRoleId(int roleId);
		vector<User> searchByBirthYear(int year);

		// Sort Functions
		vector<User> getAllSortedByUserName(bool ascending = true);
		vector<User> getAllSortedByFullName(bool ascending = true);
		vector<User> getAllSortedByGender(bool ascending = true);
		vector<User> getAllSortedByBirth(bool ascending = true);
		vector<User> getAllSortedByRole(bool ascending = true);
};

#endif