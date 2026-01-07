#ifndef BILL_BLL_H
#define BILL_BLL_H
#include "../DAL/BillDAL.h"
#include "../DAL/FoodDAL.h"
#include "../DAL/TableDAL.h"
#include "Result.h"
#include <string>
#include <vector>

class BillBLL {
    private:
        BillDAL &billDal;
        TableDAL &tableDal;
        FoodDAL &foodDal;

        Result validateTableId(int tableId);
        Result validateFoodId(int foodId);
        Result validateQuantity(int quantity);
        Result validateBillItemId(int billItemId);

    public:
        BillBLL(BillDAL &bDal, TableDAL &tDal, FoodDAL &fDal)
            : billDal(bDal), tableDal(tDal), foodDal(fDal) {}

        // Bill Operations
        bool getOpenBillByTableId(int tableId, Bill &out);
        bool getCurrentItemsOfTable(int tableId, vector<BillItem> &outItems,
                                    Bill *outBill = NULL, double *outTotal = NULL);
        Result checkoutTable(int tableId);

        Result addFoodToTable(int tableId, int foodId, int quantity, const string &desc);
        Result updateBillItem(int billItemId, int quantity, const string &desc);
        Result deleteBillItem(int billItemId);

        vector<Bill> getAll();
        bool getById(int billId, Bill &out);
        vector<Bill> searchByTableNumber(int tableNumber);
        vector<Bill> searchByDateRange(const string &startDate, const string &endDate);
        vector<Bill> searchByStatus(bool isPaid);
        vector<Bill> searchByPriceRange(double minPrice, double maxPrice);

        vector<Bill> getAllSortedByBillId(bool ascending = true);
        vector<Bill> getAllSortedByTableNumber(bool ascending = true);
        vector<Bill> getAllSortedByTotalPrice(bool ascending = true);
        vector<Bill> getAllSortedByPaidDate(bool ascending = true);
};

#endif
