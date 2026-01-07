#ifndef USER_H
#define USER_H
#include <string>
using namespace std;

struct User {
    int id;
    string userName;
    string password;
    string fullName;
    string phoneNumber;
    string birth;
    int genderId;
    int roleId;
};
#endif

