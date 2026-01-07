#include "BillBLL.h"
#include <stdexcept>

Result BillBLL::validateTableId(int tableId) {
    if (tableId <= 0) {
        return Result(false, "Table ID khong hop le");
    }
    Table tempTable;
    if (!tableDal.getById(tableId, tempTable)) {
        return Result(false, "Ban khong ton tai");
    }
    return Result(true, "");
}

Result BillBLL::validateFoodId(int foodId) {
    if (foodId <= 0) {
        return Result(false, "Food ID khong hop le");
    }
    Food tempFood;
    if (!foodDal.getById(foodId, tempFood)) {
        return Result(false, "Mon an khong ton tai");
    }
    return Result(true, "");
}

Result BillBLL::validateQuantity(int quantity) {
    if (quantity <= 0) {
        return Result(false, "So luong phai lon hon 0");
    }
    if (quantity > 100) {
        return Result(false, "So luong qua lon (toi da 100)");
    }
    return Result(true, "");
}

Result BillBLL::validateBillItemId(int billItemId) {
    if (billItemId <= 0) {
        return Result(false, "Bill Item ID khong hop le");
    }
    return Result(true, "");
}

bool BillBLL::getOpenBillByTableId(int tableId, Bill &out) {
    try {
        return billDal.getOpenByTableId(tableId, out);
    } catch (const exception &e) {
        return false;
    }
}

bool BillBLL::getCurrentItemsOfTable(int tableId, vector<BillItem> &outItems,
                                     Bill *outBill, double *outTotal) {
    try {
        Bill b;
        if (!billDal.getOpenByTableId(tableId, b))
            return false;

        outItems.clear();
        billDal.listItemsByBill(b.id, outItems);
        billDal.recalcTotal(b.id);

        if (!billDal.getOpenByTableId(tableId, b))
            return false;

        if (outBill)
            *outBill = b;
        if (outTotal)
            *outTotal = b.total_price;

        return true;
    } catch (const exception &e) {
        return false;
    }
}

Result BillBLL::checkoutTable(int tableId) {
    Result r;
    try {
        Bill b;
        if (!billDal.getOpenByTableId(tableId, b)) {
            r.ok = false;
            r.message = "Khong co bill nao dang mo";
            return r;
        }

        billDal.recalcTotal(b.id);
        billDal.closeBill(b.id);

        Table t;
        if (tableDal.getById(tableId, t)) {
            t.status_id = 0;
            tableDal.update(t);
        }

        r.ok = true;
        r.message = "Thanh toan thanh cong";
        return r;
    } catch (const exception &e) {
        r.ok = false;
        r.message = "Loi thanh toan: " + string(e.what());
        return r;
    }
}

Result BillBLL::addFoodToTable(int tableId, int foodId, int quantity, const string &desc) {
    Result r;
    try {

        r = validateTableId(tableId);
        if (!r.ok)
            return r;

        r = validateFoodId(foodId);
        if (!r.ok)
            return r;

        r = validateQuantity(quantity);
        if (!r.ok)
            return r;

        Bill b;
        int billId = -1;
        if (billDal.getOpenByTableId(tableId, b)) {
            billId = b.id;
        } else {
            billId = billDal.createForTable(tableId);
            if (billId <= 0) {
                r.ok = false;
                r.message = "Tao bill that bai";
                return r;
            }
        }

        int newItemId = -1;
        if (!billDal.addItem(billId, foodId, quantity, desc, newItemId)) {
            r.ok = false;
            r.message = "Them mon vao bill that bai";
            return r;
        }

        billDal.recalcTotal(billId);

        Table t;
        if (tableDal.getById(tableId, t)) {
            t.status_id = 1;
            tableDal.update(t);
        }

        r.ok = true;
        r.message = "Them mon vao bill thanh cong";
        return r;
    } catch (const exception &e) {
        r.ok = false;
        r.message = "Loi them mon: " + string(e.what());
        return r;
    }
}

Result BillBLL::updateBillItem(int billItemId, int quantity, const string &desc) {
    Result r;
    try {
        r = validateBillItemId(billItemId);
        if (!r.ok)
            return r;

        r = validateQuantity(quantity);
        if (!r.ok)
            return r;

        if (!billDal.updateItem(billItemId, quantity, desc)) {
            r.ok = false;
            r.message = "Cap nhat that bai";
            return r;
        }

        int billId = 0, tableId = 0;
        if (billDal.getItemContext(billItemId, billId, tableId)) {
            billDal.recalcTotal(billId);
        }

        r.ok = true;
        r.message = "Cap nhat thanh cong";
        return r;
    } catch (const exception &e) {
        r.ok = false;
        r.message = "Loi cap nhat: " + string(e.what());
        return r;
    }
}

Result BillBLL::deleteBillItem(int billItemId) {
    Result r;
    try {
        r = validateBillItemId(billItemId);
        if (!r.ok)
            return r;

        int billId = 0, tableId = 0;
        if (!billDal.getItemContext(billItemId, billId, tableId)) {
            r.ok = false;
            r.message = "Khong tim thay de xoa";
            return r;
        }

        if (!billDal.removeItem(billItemId)) {
            r.ok = false;
            r.message = "Xoa that bai";
            return r;
        }

        int cnt = billDal.countItems(billId);
        if (cnt <= 0) {
            billDal.deleteBill(billId);

            Table t;
            if (tableDal.getById(tableId, t)) {
                t.status_id = 0;
                tableDal.update(t);
            }
        } else {
            billDal.recalcTotal(billId);
        }

        r.ok = true;
        r.message = "Xoa thanh cong";
        return r;
    } catch (const exception &e) {
        r.ok = false;
        r.message = "Loi xoa mon: " + string(e.what());
        return r;
    }
}

vector<Bill> BillBLL::getAll() {
    try {
        return billDal.getAll();
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

bool BillBLL::getById(int billId, Bill &out) {
    if (billId <= 0)
        return false;
    try {
        return billDal.getById(billId, out);
    } catch (const exception &e) {
        return false;
    }
}

vector<Bill> BillBLL::searchByTableNumber(int tableNumber) {
    try {
        return billDal.searchByTableNumber(tableNumber);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::searchByDateRange(const string &startDate, const string &endDate) {
    try {
        return billDal.searchByDateRange(startDate, endDate);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::searchByStatus(bool isPaid) {
    try {
        return billDal.searchByStatus(isPaid);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::searchByPriceRange(double minPrice, double maxPrice) {
    if (minPrice < 0 || maxPrice < 0 || minPrice > maxPrice) {
        return vector<Bill>();
    }

    try {
        return billDal.searchByPriceRange(minPrice, maxPrice);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::getAllSortedByBillId(bool ascending) {
    try {
        return billDal.getAllSortedByBillId(ascending);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::getAllSortedByTableNumber(bool ascending) {
    try {
        return billDal.getAllSortedByTableNumber(ascending);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::getAllSortedByTotalPrice(bool ascending) {
    try {
        return billDal.getAllSortedByTotalPrice(ascending);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}

vector<Bill> BillBLL::getAllSortedByPaidDate(bool ascending) {
    try {
        return billDal.getAllSortedByPaidDate(ascending);
    } catch (const exception &e) {
        return vector<Bill>();
    }
}