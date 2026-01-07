#ifndef FOOD_BLL_H
#define FOOD_BLL_H
#include "../DAL/CategoryDAL.h"
#include "../DAL/FoodDAL.h"
#include "../Models/Food.h"
#include "BaseBLL.h"

class FoodBLL : public BaseBLL<Food, FoodDAL> {
    private:
        CategoryDAL &categoryDal;
        Result validateName(const string &name);
        Result validateCategory(int categoryId);
        Result validatePrice(double price);

    protected:
        Result validateEntity(const Food &entity, bool isUpdate) override;
        string getEntityName() const override;

    public:
        FoodBLL(FoodDAL &fDal, CategoryDAL &cDal) : BaseBLL(fDal), categoryDal(cDal) {};
        vector<Food> searchByName(const string &keyword);
        vector<Food> searchByCategory(const string &categoryName);
        vector<Food> searchByPriceRange(double minPrice, double maxPrice);
        vector<Food> getAllSortedByName(bool ascending = true);
        vector<Food> getAllSortedByPrice(bool ascending = true);
        vector<Food> getAllSortedByCategory(bool ascending = true);
};
#endif