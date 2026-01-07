#ifndef TABLE_DAL_H
#define TABLE_DAL_H
#include "BaseDAL.h"
#include "../Models/Table.h"
#include <vector>
using namespace std;

class TableDAL : public BaseDAL {
	private:
		SQLINTEGER id, number, capacity, status_id;
		SQLLEN idLen, numberLen, capacityLen, statusLen;

		void bindTableColumns(SQLHANDLE stmt) {
			bindColumnInt(stmt, 1, id, idLen);
			bindColumnInt(stmt, 2, number, numberLen);
			bindColumnInt(stmt, 3, capacity, capacityLen);
			bindColumnInt(stmt, 4, status_id, statusLen);
		}

		Table fetchTable() {
			Table t;
			t.id = id;
			t.number = number;
			t.capacity = capacity;
			t.status_id = status_id;
			return t;
		}

		string getBaseQuery() const {
			return 	"SELECT t.table_id, t.table_number, t.capacity, t.status_id "
			        "FROM tables t";
		}

	public:
		vector<Table> getAll();
		bool getById(int id, Table& out);
		bool insert(const Table& table);
		bool update(const Table& table);
		bool remove(int id);

		vector<Table> searchByNumber(int);
		vector<Table> searchByCapacity(int);
		vector<Table> searchByStatus(int);

		vector<Table> getAllSortedByNumber(bool ascending = true);
		vector<Table> getAllSortedByCapacity(bool ascending = true);
		vector<Table> getAllSortedByStatus(bool ascending = true);
};
#endif
