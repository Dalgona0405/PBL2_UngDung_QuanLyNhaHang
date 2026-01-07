#ifndef FOOD_H
#define FOOD_H
#include <string>
using namespace std;

struct Food {
    int id;
    string name;
    int categoryId;
    string categoryName;
    double price;
};
#endif

