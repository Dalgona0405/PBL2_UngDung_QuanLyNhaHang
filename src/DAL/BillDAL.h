#ifndef BILL_DAL_H
#define BILL_DAL_H
#include "BaseDAL.h"
#include "../Models/Bill.h"
#include "../Models/BillItem.h"
#include <string>
#include <vector>

class BillDAL : public BaseDAL {
	private:
	    SQLINTEGER billId, tableId;
	    SQLDOUBLE totalPrice;
	    SQLCHAR paidDate[32];
	    SQLLEN billIdLen, tableIdLen, totalPriceLen, paidDateLen;
	    
	    void bindBillColumns(SQLHANDLE stmt) {
	        bindColumnInt(stmt, 1, billId, billIdLen);
	        bindColumnInt(stmt, 2, tableId, tableIdLen);
	        bindColumnDouble(stmt, 3, totalPrice, totalPriceLen);
	        bindColumnString(stmt, 4, paidDate, sizeof(paidDate), paidDateLen);
	    }
	    
	    Bill fetchBill() {
	        Bill bill;
	        bill.id = billId;
	        bill.table_id = tableId;
	        bill.total_price = totalPrice;
	        bill.paid_date = getString(paidDate, paidDateLen);
	        return bill;
	    }
	    
	    string getBaseQuery() const {
	        return  "SELECT bill_id, table_id, total_price, paid_date "
					"FROM bills";
	    }
	    
	    bool getFoodPrice(int foodId, double &price);
	
	public:
	    // CRUD Operations
	    bool getOpenByTableId(int tableId, Bill &out);
	    int createForTable(int tableId);
	    bool deleteBill(int billId);
	    bool recalcTotal(int billId);
	    
	    // Bill Item Operations
	    bool addItem(int billId, int foodId, int quantity, const string &desc, int &outNewId);
	    bool updateItem(int billItemId, int quantity, const string &desc);
	    bool removeItem(int billItemId);
	    bool getItemContext(int billItemId, int &billId, int &tableId);
	    bool listItemsByBill(int billId, vector<BillItem> &out);
	    
	    // Bill Operations
	    bool closeBill(int billId);
	    int countItems(int billId);

	    vector<Bill> getAll();
	    bool getById(int billId, Bill& out);
	    vector<Bill> searchByTableNumber(int tableNumber);
	    vector<Bill> searchByDateRange(const string& startDate, const string& endDate);
	    vector<Bill> searchByStatus(bool isPaid); // true = paid, false = unpaid
	    vector<Bill> searchByPriceRange(double minPrice, double maxPrice);

	    vector<Bill> getAllSortedByBillId(bool ascending = true);
	    vector<Bill> getAllSortedByTableNumber(bool ascending = true);
	    vector<Bill> getAllSortedByTotalPrice(bool ascending = true);
	    vector<Bill> getAllSortedByPaidDate(bool ascending = true);
};

#endif
