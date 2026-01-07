#ifndef CATEGORY_DAL_H
#define CATEGORY_DAL_H
#include "BaseDAL.h"
#include "../Models/Category.h"
#include <vector>
using namespace std;

class CategoryDAL : public BaseDAL {
	private:
		SQLINTEGER id;
		SQLCHAR name[256];
		SQLLEN idLen, nameLen;

		void bindCategoryColumns(SQLHANDLE stmt) {
			bindColumnInt(stmt, 1, id, idLen);
			bindColumnString(stmt, 2, name, sizeof(name), nameLen);
		}

		Category fetchCategory() {
			Category c;
			c.id = id;
			c.name = getString(name, nameLen);
			return c;
		}

		string getBaseQuery() const {
			return "SELECT c.category_id, c.category_name FROM categories c";
		}

	public:
		vector<Category> getAll();
		bool getById(int id, Category& out);
		bool insert(const Category&);
		bool update(const Category&);
		bool remove(int);

		vector<Category> searchByName(const string&);
		vector<Category> getAllSortedByName(bool ascending = true);
};
#endif