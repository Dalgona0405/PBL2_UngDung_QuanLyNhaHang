#ifndef RESULT_H
#define RESULT_H
#include <string>
using namespace std;

struct Result {
        bool ok = false;
        string message = "";
        Result() = default;
        Result(bool success, const string &msg) : ok(success), message(msg) {}
};
#endif