#ifndef USER_BLL_H
#define USER_BLL_H
#include "../DAL/UserDAL.h"
#include "../Models/User.h"
#include "BaseBLL.h"

class UserBLL : public BaseBLL<User, UserDAL> {
    private:
        User currentUser;

        Result validateUserName(const string &s);
        Result validatePassword(const string &s);
        Result validateFullName(const string &s);
        Result validateRole(int roleId);
        Result validateGender(int genderId);
        Result validateBirth(const string &birth);

    protected:
        Result validateEntity(const User &entity, bool isUpdate) override;
        string getEntityName() const override;

    public:
        UserBLL(UserDAL &uDal) : BaseBLL(uDal) {
            currentUser = User();
        }
        vector<User> getAll() override;
        bool getById(int id, User &out) override;
        Result addChecked(const User &entity) override;
        Result updateChecked(const User &entity) override;
        Result removeChecked(int id) override;

        Result login(const string &username, const string &password);
        User getCurrentUser();
        void logOut();
        string genderToString(int genderId);
        string roleToString(int roleId);

        vector<User> searchByUserName(const string &keyword);
        vector<User> searchByFullName(const string &keyword);
        vector<User> searchByPhone(const string &keyword);
        vector<User> searchByGenderId(int genderId);
        vector<User> searchByRoleId(int roleId);
        vector<User> searchByBirthYear(int year);

        vector<User> getAllSortedByUserName(bool ascending = true);
        vector<User> getAllSortedByFullName(bool ascending = true);
        vector<User> getAllSortedByGender(bool ascending = true);
        vector<User> getAllSortedByBirth(bool ascending = true);
        vector<User> getAllSortedByRole(bool ascending = true);
};
#endif
