#include "CategoryBLL.h"

Result CategoryBLL::validateName(const string &name) {
    if (name.size() == 0) {
        return Result(false, string("Ten khong duoc rong"));
    }
    if (name.size() > 100) {
        return Result(false, string("Ten danh muc qua dai (<=100 ky tu)"));
    }
    return Result(true, string(""));
}

Result CategoryBLL::validateEntity(const Category &entity, bool isUpdate) {
    return validateName(entity.name);
}

string CategoryBLL::getEntityName() const {
    return "danh muc";
}

Result CategoryBLL::removeChecked(int id) {
    Result r;
    try {
        Category exists;
        if (!getById(id, exists)) {
            r.ok = false;
            r.message = "Danh muc khong ton tai";
            return r;
        }
        if (!foodDal.setCategoryIdToNull(id)) {
            r.ok = false;
            r.message = "Loi set null categoryid";
        }
        if (!dal.remove(id)) {
            r.ok = false;
            r.message = "Xoa danh muc that bai";
            return r;
        }
        r.ok = true;
        r.message = "Xoa danh muc thanh cong";
    } catch (const exception &e) {
        r.ok = false;
        r.message = "Loi xoa: " + string(e.what());
    }
    return r;
}

vector<Category> CategoryBLL::searchByName(const string &keyword) {
    try {
        return dal.searchByName(keyword);
    } catch (const exception &e) {
        return vector<Category>();
    }
}

vector<Category> CategoryBLL::getAllSortedByName(bool ascending) {
    try {
        return dal.getAllSortedByName(ascending);
    } catch (const exception &e) {
        return vector<Category>();
    }
}