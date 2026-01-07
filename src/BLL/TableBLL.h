#ifndef TABLE_BLL_H
#define TABLE_BLL_H
#include "../DAL/TableDAL.h"
#include "../Models/Table.h"
#include "BaseBLL.h"

class TableBLL : public BaseBLL<Table, TableDAL> {
    private:
        Result validateNumber(int number, int excludeId = -1);
        Result validateCapacity(int capacity);
        Result validateStatus(int status_id);

    protected:
        Result validateEntity(const Table &entity, bool isUpdate) override;
        string getEntityName() const override;

    public:
        TableBLL(TableDAL &tDal) : BaseBLL(tDal) {};
        vector<Table> searchByNumber(int number);
        vector<Table> searchByCapacity(int capacity);
        vector<Table> searchByStatus(int status);
        vector<Table> getAllSortedByNumber(bool ascending = true);
        vector<Table> getAllSortedByCapacity(bool ascending = true);
        vector<Table> getAllSortedByStatus(bool ascending = true);
};
#endif