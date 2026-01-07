#ifndef BILL_H
#define BILL_H
#include <string>

struct Bill {
        int id;
        int table_id;
        double total_price;
        string paid_date; // yyyy-mm-dd hh:mm:ss
};
#endif
