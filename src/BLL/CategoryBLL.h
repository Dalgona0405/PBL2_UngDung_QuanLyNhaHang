#ifndef CATEGORY_BLL_H
#define CATEGORY_BLL_H
#include "../DAL/CategoryDAL.h"
#include "../DAL/FoodDAL.h"
#include "../Models/Category.h"
#include "BaseBLL.h"

class CategoryBLL : public BaseBLL<Category, CategoryDAL> {
    private:
        FoodDAL &foodDal;
        Result validateName(const string &name);

    protected:
        Result validateEntity(const Category &category, bool isUpdate) override;
        string getEntityName() const override;

    public:
        CategoryBLL(CategoryDAL &cDal, FoodDAL &fDal) : BaseBLL(cDal), foodDal(fDal) {};
        Result removeChecked(int id) override;
        vector<Category> searchByName(const string &keyword);
        vector<Category> getAllSortedByName(bool ascending = true);
};
#endif