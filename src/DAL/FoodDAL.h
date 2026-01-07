#ifndef FOOD_DAL_H
#define FOOD_DAL_H
#include "../Models/Food.h"
#include "BaseDAL.h"
#include <vector>
using namespace std;

class FoodDAL : public BaseDAL {
    private:
        SQLINTEGER id, category_id;
        SQLCHAR name[256], category_name[256];
        SQLDOUBLE price;
        SQLLEN idLen, nameLen, category_idLen, category_nameLen, priceLen;

        void bindFoodColumns(SQLHANDLE stmt) {
            bindColumnInt(stmt, 1, id, idLen);
            bindColumnString(stmt, 2, name, sizeof(name), nameLen);
            bindColumnInt(stmt, 3, category_id, category_idLen);
            bindColumnString(stmt, 4, category_name, sizeof(category_name), category_nameLen);
            bindColumnDouble(stmt, 5, price, priceLen);
        }

        Food fetchFood() {
            Food f;
            f.id = id;
            f.name = getString(name, nameLen);
            f.categoryId = isNull(category_idLen) ? 0 : category_id;
            f.categoryName = getString(category_name, category_nameLen);
            f.price = price;
            return f;
        }

        string getBaseQuery() const {
            return "SELECT f.food_id, f.food_name, f.category_id, c.category_name, f.price "
                   "FROM foods f LEFT JOIN categories c ON f.category_id = c.category_id";
        }

    public:
        // CRUD
        vector<Food> getAll();
        bool getById(int id, Food &out);
        bool insert(const Food &food);
        bool update(const Food &food);
        bool remove(int idQuery);
        bool setCategoryIdToNull(int categoryId);
        // Search
        vector<Food> searchByName(const string &keyword);
        vector<Food> searchByCategory(const string &categoryName);
        vector<Food> searchByPriceRange(double minPrice, double maxPrice);
        // Sort
        vector<Food> getAllSortedByName(bool ascending = true);
        vector<Food> getAllSortedByPrice(bool ascending = true);
        vector<Food> getAllSortedByCategory(bool ascending = true);
};

#endif