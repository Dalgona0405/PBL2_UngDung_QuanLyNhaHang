// UserUI.h
#pragma once
#ifndef USER_UI_H
#define USER_UI_H

#include "../BLL/UserBLL.h"
#include "PagedListScreen.h"
#include <string>

// Man hinh dang nhap: Username / Password / Dang nhap
class UserLoginScreen : public Screen {
    private:
        UserBLL &userBLL_;

        string username_;
        string password_;
        int focus_; // 0 = username, 1 = password, 2 = login
        string msg_;
        mutable short userRow_;
        mutable short passRow_;
        mutable short actionRow_;
        mutable short msgRow_;

        void drawForm() const;
        void redrawFocusLines();
        void showMsg();

        void editUsername();
        void editPassword();
        void doLogin();

    public:
        UserLoginScreen(App *a);

        void draw() override;
        void onKey(Key k) override;
};

// Simple profile viewing screen for current user
class UserProfileScreen : public Screen {
    private:
        UserBLL &userBLL_;
        User user_;

    public:
        UserProfileScreen(App *a);
        void draw() override;
        void onKey(Key k) override;
};

// Quan ly nhan su (Admin only)
class UserListScreen : public PagedListScreen<User> {
    private:
        UserBLL &userBLL_;

        bool selectMode_;
        bool inAction_;
        int actionIndex_;
        string msg_;
        bool showingMsg_;
        string viewNote_;
        int msgRow_;

        enum class UserFilterType { None,
                                    UserName,
                                    FullName,
                                    Phone,
                                    Gender,
                                    Role,
                                    BirthYear };
        enum class UserSortField { None,
                                   UserName,
                                   FullName,
                                   Gender,
                                   Birth,
                                   Role };
        enum class UserAction { Add,
                                Search,
                                Sort };

        UserFilterType filterType_;
        UserSortField sortField_;
        bool sortAsc_;
        string keyword_;
        int filterGender_;
        int filterRole_;
        int filterBirthYear_;

        int topActionRow_;
        int topActionIndex_;
        bool onTopAction_;

        void applySort();
        void resetView();

        bool promptStringInput(const string &title, const string &label, string &out);
        bool promptIntInput(const string &title, const string &label, int &out);
        bool promptGender(int &gender);
        bool promptRole(int &role);
        bool promptSortOrder(bool &asc);

        void openSearchMenu();
        void openSortMenu();
        vector<UserAction> topActions() const;
        void drawTopActions();
        void handleTopAction(UserAction action);

    public:
        UserListScreen(App *a);
        void draw() override;
        void onKey(Key k) override;
        void reloadData() override;
        void drawTableHeader() override;
        void drawRowItem(int index, const User &item, bool selected) override;
};

class UserEditScreen : public Screen {
    private:
        UserBLL &userBLL_;
        bool isNew_;
        User user_;
        int focus_; // 0 username,1 password,2 fullname,3 phone,4 birth,5 gender,6 role,7 confirm
        string msg_;
        mutable short baseRow_;
        void drawForm() const;
        void redrawFocusLines() const;
        void editTextField(int focusField, const string &label, string &value, bool mask = false);
        void editBirth();
        void editGender();
        void save();

    public:
        UserEditScreen(App *a, bool isNew, const User &initial);
        void draw() override;
        void onKey(Key k) override;
};

#endif // USER_UI_H
