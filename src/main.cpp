#include "UI/ConsoleUI.h"
#include "UI/ConsoleUtils.h"

// BLL
#include "BLL/BillBLL.h"
#include "BLL/CategoryBLL.h"
#include "BLL/FoodBLL.h"
#include "BLL/StatisticsBLL.h"
#include "BLL/TableBLL.h"
#include "BLL/UserBLL.h"

// DAL
#include "DAL/BillDAL.h"
#include "DAL/CategoryDAL.h"
#include "DAL/FoodDAL.h"
#include "DAL/StatisticsDAL.h"
#include "DAL/TableDAL.h"
#include "DAL/UserDAL.h"

// UI
#include "UI/UserUI.h"

int main() {
    EnableVTMode();
    SetConsoleOutputCP(65001);
    hideCursor();
    drawAppHeader();

    CategoryDAL cDal;
    FoodDAL fDal;
    UserDAL uDal;
    TableDAL tDal;
    BillDAL bDal;
    StatisticsDAL sDal;

    CategoryBLL catBLL(cDal, fDal);
    FoodBLL foodBLL(fDal, cDal);
    UserBLL userBLL(uDal);
    TableBLL tableBLL(tDal);
    BillBLL billBLL(bDal, tDal, fDal);
    StatisticsBLL statisticsBLL(sDal);

    App app;
    app.setUserBLL(&userBLL);
    app.setFoodBLL(&foodBLL);
    app.setCategoryBLL(&catBLL);
    app.setTableBLL(&tableBLL);
    app.setBillBLL(&billBLL);
    app.setStatisticsBLL(&statisticsBLL);

    app.push(new UserLoginScreen(&app));
    app.run();

    showCursor();
    return 0;
}
