// ConsoleUI.h
#pragma once
#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "ConsoleInput.h"
#include <cstddef>
#include <vector>
using namespace std;

class App;
class FoodBLL;
class CategoryBLL;
class TableBLL;
class UserBLL;
class BillBLL;
class StatisticsBLL;

class Screen {
    protected:
        App *app;

    public:
        explicit Screen(App *a) : app(a) {}
        virtual ~Screen() {}

        virtual void draw() = 0;       // Ve full khi man hinh moi xuat hien
        virtual void onKey(Key k) = 0; // To ve delta trong nay neu can
};

class App {
        vector<Screen *> stack_;
        bool running_;

        FoodBLL *foodBLL_;
        CategoryBLL *categoryBLL_;
        TableBLL *tableBLL_;
        UserBLL *userBLL_;
        BillBLL *billBLL_;
        StatisticsBLL *statisticsBLL_;

    public:
        App() : running_(true), foodBLL_(0), categoryBLL_(0), tableBLL_(0), userBLL_(0), billBLL_(0), statisticsBLL_(0) {}
        ~App() {
            while (!stack_.empty()) {
                delete stack_.back();
                stack_.pop_back();
            }
        }

        // == Setter ==
        void setFoodBLL(FoodBLL *bll) { foodBLL_ = bll; }
        void setCategoryBLL(CategoryBLL *bll) { categoryBLL_ = bll; }
        void setTableBLL(TableBLL *bll) { tableBLL_ = bll; }
        void setUserBLL(UserBLL *bll) { userBLL_ = bll; }
        void setBillBLL(BillBLL *bll) { billBLL_ = bll; }
        void setStatisticsBLL(StatisticsBLL *bll) { statisticsBLL_ = bll; }

        // == Getter ==
        FoodBLL &food() { return *foodBLL_; }
        CategoryBLL &category() { return *categoryBLL_; }
        TableBLL &table() { return *tableBLL_; }
        UserBLL &user() { return *userBLL_; }
        BillBLL &bill() { return *billBLL_; }
        StatisticsBLL &statistics() { return *statisticsBLL_; }

        void push(Screen *s) { stack_.push_back(s); }

        void pop() {
            if (!stack_.empty()) {
                delete stack_.back();
                stack_.pop_back();
            }
            if (stack_.empty())
                running_ = false;
        }

        void replaceTop(Screen *s) {
            if (!stack_.empty()) {
                delete stack_.back();
                stack_.back() = s;
            } else {
                stack_.push_back(s);
            }
        }

        void exit() { running_ = false; }

        Screen *top() { return stack_.empty() ? NULL : stack_.back(); }

        void run() {
            if (top())
                top()->draw(); // ve full lan dau khi vao man

            while (running_ && top() != NULL) {
                Key k = readKey();
                if (k == KEY_ESC) {
                    running_ = false;
                    break;
                }

                Screen *before = top();
                before->onKey(k); // man hinh tu quyet dinh co ve delta hay goi draw()

                if (!running_ || top() == NULL)
                    break;

                // Neu co push/pop man hinh -> ve full man hinh moi
                if (top() != before) {
                    top()->draw();
                }
            }
        }
};

#endif
