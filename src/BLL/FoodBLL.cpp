#include "FoodBLL.h"

Result FoodBLL::validateName(const string &name) {
    if (name.empty()) {
        return Result(false, "Ten khong duoc rong");
    }
    if (name.size() > 100) {
        return Result(false, "Ten mon qua dai (<=100 ky tu)");
    }
    return Result(true, "");
}

Result FoodBLL::validateCategory(int categoryId) {
    if (categoryId < 0) {
        return Result(false, "CategoryID phai lon hon 0");
    }
    Category tempCat;
    if (!categoryDal.getById(categoryId, tempCat)) {
        return Result(false, "Danh muc khong ton tai");
    }
    return Result(true, "");
}

Result FoodBLL::validatePrice(double price) {
    if (price <= 0) {
        return Result(false, "Gia phai lon hon 0");
    }
    if (price > 100000000) {
        return Result(false, "Gia qua lon");
    }
    return Result(true, "");
}

Result FoodBLL::validateEntity(const Food &entity, bool isUpdate) {
    Result r;
    r = validateName(entity.name);
    if (!r.ok)
        return r;
    vector<Food> potentialDuplicates = dal.searchByName(entity.name);
	for (size_t i = 0; i < potentialDuplicates.size(); ++i) {
		const Food& f = potentialDuplicates[i];
		if (f.name == entity.name) {
			if (isUpdate && f.id == entity.id) {
				continue;  // Cho phép giữ nguyên username khi update
			}
			return Result(false, "Ten mon an da ton tai, vui long chon ten khac");
		}
	}
    r = validateCategory(entity.categoryId);
    if (!r.ok)
        return r;

    r = validatePrice(entity.price);
    return r;
}

string FoodBLL::getEntityName() const {
    return "mon an";
}

vector<Food> FoodBLL::searchByName(const string &keyword) {
    try {
        return dal.searchByName(keyword);
    } catch (const exception &e) {
        return vector<Food>();
    }
}

vector<Food> FoodBLL::searchByCategory(const string &categoryName) {
    try {
        return dal.searchByCategory(categoryName);
    } catch (const exception &e) {
        return vector<Food>();
    }
}

vector<Food> FoodBLL::searchByPriceRange(double minPrice, double maxPrice) {
    if (minPrice < 0 || maxPrice < 0 || minPrice > maxPrice) {
        return vector<Food>();
    }
    try {
        return dal.searchByPriceRange(minPrice, maxPrice);
    } catch (const exception &e) {
        return vector<Food>();
    }
}

vector<Food> FoodBLL::getAllSortedByName(bool ascending) {
    try {
        return dal.getAllSortedByName(ascending);
    } catch (const exception &e) {
        return vector<Food>();
    }
}

vector<Food> FoodBLL::getAllSortedByCategory(bool ascending) {
    try {
        return dal.getAllSortedByCategory(ascending);
    } catch (const exception &e) {
        return vector<Food>();
    }
}

vector<Food> FoodBLL::getAllSortedByPrice(bool ascending) {
    try {
        return dal.getAllSortedByPrice(ascending);
    } catch (const exception &e) {
        return vector<Food>();
    }
}