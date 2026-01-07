#ifndef BILL_ITEM_H
#define BILL_ITEM_H
#include <string>

struct BillItem {
    int id;
    int bill_id;
    int food_id;
    int quantity;
    string description;
    double sub_total;
};
#endif
