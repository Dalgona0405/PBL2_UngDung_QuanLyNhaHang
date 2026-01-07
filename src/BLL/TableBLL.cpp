#include "TableBLL.h"

Result TableBLL::validateNumber(int number, int excludeId) {
	if (number <= 0) {
        return Result(false, "So ban phai lon hon 0");
    }
    vector<Table> tables = dal.searchByNumber(number);
    for (size_t i = 0; i < tables.size(); ++i) {
        if (tables[i].id != excludeId) {
            return Result(false, "So ban da ton tai");
        }
    }
    return Result(true, "");
}

Result TableBLL::validateCapacity(int capacity) {
    if (capacity <= 0) {
        return Result(false, "So cho ngoi phai lon hon 0");
    }
    if (capacity > 20) {
        return Result(false, "So cho ngoi qua lon (<=20 cho ngoi)");
    }
    return Result(true, "");
}

Result TableBLL::validateStatus(int status_id) {
    if (status_id != 0 && status_id != 1) {
        return Result(false, "Status chi duoc phep la 0 (trong) hoac 1 (co khach)");
    }
    return Result(true, "");
}

Result TableBLL::validateEntity(const Table &entity, bool isUpdate) {
    Result r;
    int excludeId = isUpdate ? entity.id : -1;
    r = validateNumber(entity.number, excludeId);
    if (!r.ok)
        return r;

    r = validateCapacity(entity.capacity);
    if (!r.ok)
        return r;

    r = validateStatus(entity.status_id);
    return r;
}

string TableBLL::getEntityName() const {
    return "ban";
}

vector<Table> TableBLL::searchByNumber(int number) {
    try {
        return dal.searchByNumber(number);
    } catch (const exception &e) {
        return vector<Table>();
    }
}

vector<Table> TableBLL::searchByCapacity(int capacity) {
    try {
        return dal.searchByCapacity(capacity);
    } catch (const exception &e) {
        return vector<Table>();
    }
}

vector<Table> TableBLL::searchByStatus(int status) {
    try {
        return dal.searchByStatus(status);
    } catch (const exception &e) {
        return vector<Table>();
    }
}

vector<Table> TableBLL::getAllSortedByCapacity(bool ascending) {
    try {
        return dal.getAllSortedByCapacity(ascending);
    } catch (const exception &e) {
        return vector<Table>();
    }
}

vector<Table> TableBLL::getAllSortedByNumber(bool ascending) {
    try {
        return dal.getAllSortedByNumber(ascending);
    } catch (const exception &e) {
        return vector<Table>();
    }
}

vector<Table> TableBLL::getAllSortedByStatus(bool ascending) {
    try {
        return dal.getAllSortedByStatus(ascending);
    } catch (const exception &e) {
        return vector<Table>();
    }
}