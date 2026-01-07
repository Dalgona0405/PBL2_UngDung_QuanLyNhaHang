#include "UserBLL.h"
#include <cctype>

Result UserBLL::validateUserName(const string& s) {
	if (s.size() == 0) {
		return Result(false, "Username khong duoc rong");
	}
	if (s.size() > 50) {
		return Result(false, "Username qua dai");
	}
	return Result(true, "");
}

Result UserBLL::validatePassword(const string& s) {
	if (s.size() == 0) {
		return Result(false, "Password khong duoc rong");
	}
	if (s.size() > 50) {
		return Result(false, "Password qua dai");
	}
	return Result(true, "");
}

Result UserBLL::validateFullName(const string& s) {
	if (s.size() == 0) {
		return Result(false, "Ho ten khong duoc rong");
	}
	return Result(true, "");
}

Result UserBLL::validateRole(int roleId) {
	if (roleId != 0 && roleId != 1) {
		return Result(false, "Role chi nhan 0 hoac 1");
	}
	return Result(true, "");
}

Result UserBLL::validateGender(int genderId) {
	if (genderId != 0 && genderId != 1) {
		return Result(false, "Gender chi nhan 0 hoac 1");
	}
	return Result(true, "");
}

static bool isLeap(int y) {
	return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

static bool parseDate(const string& s, int& d, int& m, int& y) {
	if (s.size() != 10 || s[4] != '-' || s[7] != '-')
		return false;
	for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) {
		if (!isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}
	y = stoi(s.substr(0, 4));
	m = stoi(s.substr(5, 2));
	d = stoi(s.substr(8, 2));
	if (m < 1 || m > 12 || y < 1900 || y > 2100 || d < 1)
		return false;
	int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int days = mdays[m - 1];
	if (m == 2 && isLeap(y))
		days = 29;
	return d <= days;
}

Result UserBLL::validateBirth(const string& birth) {
	int d = 0, m = 0, y = 0;
	if (!parseDate(birth, d, m, y)) {
		return Result(false, "Ngay sinh khong hop le (yyyy-mm-dd)");
	}
	return Result(true, "");
}

Result UserBLL::validateEntity(const User& entity, bool isUpdate) {
	Result r;
	r = validateUserName(entity.userName);
	if (!r.ok)
		return r;
	vector<User> potentialDuplicates = dal.searchByUserName(entity.userName);
	for (size_t i = 0; i < potentialDuplicates.size(); ++i) {
		const User& u = potentialDuplicates[i];
		if (u.userName == entity.userName) {
			if (isUpdate && u.id == entity.id) {
				continue;  // Cho phép giữ nguyên username khi update
			}
			return Result(false, "Ten dang nhap da ton tai, vui long chon ten khac");
		}
	}
	r = validatePassword(entity.password);
	if (!r.ok)
		return r;
	r = validateFullName(entity.fullName);
	if (!r.ok)
		return r;
	r = validateBirth(entity.birth);
	if (!r.ok)
		return r;
	r = validateGender(entity.genderId);
	if (!r.ok)
		return r;
	r = validateRole(entity.roleId);
	if (!r.ok)
		return r;
	
	return Result(true, "");
}

string UserBLL::getEntityName() const {
	return "nhan vien";
}

vector<User> UserBLL::getAll() {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	return BaseBLL::getAll();
}

bool UserBLL::getById(int id, User& out) {
	if (currentUser.roleId != 1) {
		return false;
	}
	return BaseBLL::getById(id, out);
}

Result UserBLL::addChecked(const User& entity) {
	Result r;
	if (currentUser.roleId != 1) {
		r.ok = false;
		r.message = "Khong co quyen";
		return r;
	}
	return BaseBLL::addChecked(entity);
}

Result UserBLL::updateChecked(const User& entity) {
	Result r;
	if (currentUser.roleId != 1) {
		r.ok = false;
		r.message = "Khong co quyen";
		return r;
	}
	return BaseBLL::updateChecked(entity);
}

Result UserBLL::removeChecked(int id) {
	Result r;
	if (currentUser.roleId != 1) {
		r.ok = false;
		r.message = "Khong co quyen";
		return r;
	}
	return BaseBLL::removeChecked(id);
}

Result UserBLL::login(const string& username, const string& password) {
	Result r;
	currentUser = User();
	if (username.empty() || password.empty()) {
		r.ok = false;
		r.message = "Ten dang nhap va mat khau khong duoc rong";
		return r;
	}
	try {
		User u;
		if (dal.login(username, password, u)) {
			currentUser = u;
			r.ok = true;
			r.message = "Dang nhap thanh cong";
		} else {
			r.ok = false;
			r.message = "Sai ten dang nhap hoac mat khau";
		}
	} catch (const exception& e) {
		r.ok = false;
		r.message = "Loi he thong: " + string(e.what());
	}
	return r;
}

User UserBLL::getCurrentUser() {
	return currentUser;
}

void UserBLL::logOut() {
	currentUser = User();
}

string UserBLL::genderToString(int genderId) {
	return genderId == 1 ? "Nam" : "Nu";
}

string UserBLL::roleToString(int roleId) {
	return roleId == 1 ? "Admin" : "Nhan vien";
}

vector<User> UserBLL::searchByUserName(const string& keyword) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByUserName(keyword);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::searchByFullName(const string& keyword) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByFullName(keyword);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::searchByPhone(const string& keyword) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByPhone(keyword);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::searchByGenderId(int genderId) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByGenderId(genderId);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::searchByRoleId(int roleId) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByRoleId(roleId);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::searchByBirthYear(int year) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.searchByBirthYear(year);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::getAllSortedByUserName(bool ascending) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.getAllSortedByUserName(ascending);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::getAllSortedByFullName(bool ascending) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.getAllSortedByFullName(ascending);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::getAllSortedByGender(bool ascending) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.getAllSortedByGender(ascending);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::getAllSortedByBirth(bool ascending) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.getAllSortedByBirth(ascending);
	} catch (const exception& e) {
		return vector<User>();
	}
}

vector<User> UserBLL::getAllSortedByRole(bool ascending) {
	if (currentUser.roleId != 1) {
		return vector<User>();
	}
	try {
		return dal.getAllSortedByRole(ascending);
	} catch (const exception& e) {
		return vector<User>();
	}
}
