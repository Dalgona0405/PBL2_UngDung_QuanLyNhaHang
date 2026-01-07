// StatisticsUI.cpp
#include "StatisticsUI.h"
#include "ConsoleUtils.h"
#include "SearchSortUtils.h"
#include <algorithm>
#include <conio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// Helper function
static void getDefaultDateRange(string &startDate, string &endDate) {
    time_t now = time(0);
    time_t sevenDaysAgo = now - 7 * 24 * 3600;

    tm *ltm1 = localtime(&sevenDaysAgo);
    char buffer[32];
    sprintf(buffer, "%04d-%02d-%02d",
            ltm1->tm_year + 1900, ltm1->tm_mon + 1, ltm1->tm_mday);
    startDate = string(buffer);

    tm *ltm2 = localtime(&now);
    sprintf(buffer, "%04d-%02d-%02d",
            ltm2->tm_year + 1900, ltm2->tm_mon + 1, ltm2->tm_mday);
    endDate = string(buffer);
}

// Helper function
static bool selectPeriodRange(
    StatsPeriodType &periodType,
    string &startDate, string &endDate,
    int &startYear, int &startMonth,
    int &endYear, int &endMonth) {

    vector<string> options = {
        "Theo ngay (tu ngay den ngay)",
        "Theo thang (tu thang den thang)",
        "Theo nam"};

    int choice = ::selectMenu(options, "CHON KHOANG THOI GIAN");
    if (choice == -1) {
        return false;
    }

    if (choice == 0) {
        // Theo ngay
        cls();
        printTitleBox("CHON KHOANG NGAY");
        cout << "\n\n";
        printDivider('=');

        string start = startDate;
        string end = endDate;

        int r1 = ::promptInlineText(2, "Tu ngay (yyyy-mm-dd)", start);
        if (r1 != 1) {
            return false;
        }

        int r2 = ::promptInlineText(3, "Den ngay (yyyy-mm-dd)", end);
        if (r2 != 1) {
            return false;
        }

        startDate = start;
        endDate = end;
        periodType = StatsPeriodType::DateRange;
        return true;

    } else if (choice == 1) {
        // Theo thang
        cls();
        printTitleBox("CHON KHOANG THANG");
        cout << "\n\n";
        printDivider('=');

        int sy = startYear, sm = startMonth;
        int ey = endYear, em = endMonth;

        string startMonthStr = "";
        string endMonthStr = "";

        int r1 = ::promptInlineText(2, "Tu thang (yyyy-mm)", startMonthStr);
        if (r1 != 1) {
            return false;
        }

        int r2 = ::promptInlineText(3, "Den thang (yyyy-mm)", endMonthStr);
        if (r2 != 1) {
            return false;
        }

        // Parse startMonth
        size_t pos1 = startMonthStr.find('-');
        if (pos1 != string::npos) {
            sy = atoi(startMonthStr.substr(0, pos1).c_str());
            sm = atoi(startMonthStr.substr(pos1 + 1).c_str());
        }

        // Parse endMonth
        size_t pos2 = endMonthStr.find('-');
        if (pos2 != string::npos) {
            ey = atoi(endMonthStr.substr(0, pos2).c_str());
            em = atoi(endMonthStr.substr(pos2 + 1).c_str());
        }

        startYear = sy;
        startMonth = sm;
        endYear = ey;
        endMonth = em;
        periodType = StatsPeriodType::MonthRange;
        return true;

    } else if (choice == 2) {
        // Theo nam
        cls();
        printTitleBox("CHON NAM");
        cout << "\n";
        printDivider('=');

        int y = startYear;
        int r = ::promptInlineInt(2, "Nam", y);
        if (r != 1) {
            return false;
        }

        startYear = y;
        periodType = StatsPeriodType::Year;
        return true;
    }

    return false;
}

// ======================= StatisticsMenuScreen =======================
StatisticsMenuScreen::StatisticsMenuScreen(App *a)
    : Screen(a),
      statisticsBLL_(a->statistics()),
      sel_(0),
      menuStartRow_(0) {
    buildMenu();
}

void StatisticsMenuScreen::buildMenu() {
    items_.clear();
    items_.push_back("Thong ke doanh thu");
    items_.push_back("Top mon an ban chay");
    items_.push_back("Lich su giao dich");

    if (sel_ >= (int)items_.size())
        sel_ = (int)items_.size() - 1;
}

void StatisticsMenuScreen::drawItem(int index, bool selected) {
    short row = menuStartRow_ + index;
    gotoRC(row, 0);
    clearLine();
    cout << bullet(selected) << items_[index];
    cout.flush();
}

void StatisticsMenuScreen::draw() {
    cls();

    printTitleBox("THONG KE & BAO CAO", "Xem bao cao doanh thu va lich su giao dich");
    printDivider('=');
    cout << "Chon loai bao cao:\n";

    menuStartRow_ = contentRow();

    for (size_t i = 0; i < items_.size(); ++i) {
        drawItem((int)i, (int)i == sel_);
        cout << "\n";
    }

    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] xem bao cao  |  [Backspace] quay lai");
    cout.flush();
}

void StatisticsMenuScreen::onKey(Key k) {
    int old = sel_;
    if (k == KEY_UP && sel_ > 0) {
        sel_--;
    } else if (k == KEY_DOWN && sel_ + 1 < (int)items_.size()) {
        sel_++;
    } else if (k == KEY_ENTER) {
        if (sel_ == 0) {
            app->push(new RevenueStatisticsScreen(app));
            return;
        } else if (sel_ == 1) {
            app->push(new TopFoodsScreen(app));
            return;
        } else if (sel_ == 2) {
            app->push(new TransactionHistoryScreen(app));
            return;
        }
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    }

    if (sel_ != old) {
        drawItem(old, false);
        drawItem(sel_, true);
    }
}

// ======================= RevenueStatisticsScreen =======================
RevenueStatisticsScreen::RevenueStatisticsScreen(App *a)
    : Screen(a),
      statisticsBLL_(a->statistics()),
      periodType_(StatsPeriodType::DateRange),
      startYear_(0), startMonth_(0),
      endYear_(0), endMonth_(0),
      billCount_(0),
      totalRevenue_(0),
      selectedIndex_(0),
      firstRow_(7),
      msg_("") {
    getDefaultDateRange(startDate_, endDate_);
    reload();
}

void RevenueStatisticsScreen::reload() {
    billCount_ = 0;
    totalRevenue_ = 0;
    dateItems_.clear();
    monthItems_.clear();

    if (periodType_ == StatsPeriodType::DateRange) {
        dateItems_ = statisticsBLL_.getRevenueByDate(startDate_, endDate_);
        statisticsBLL_.getTotalRevenue(startDate_, endDate_, billCount_, totalRevenue_);
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        monthItems_ = statisticsBLL_.getRevenueByMonthRange(startYear_, startMonth_, endYear_, endMonth_);
        for (const auto &item : monthItems_) {
            billCount_ += item.billCount;
            totalRevenue_ += item.totalRevenue;
        }
    } else if (periodType_ == StatsPeriodType::Year) {
        monthItems_ = statisticsBLL_.getRevenueByMonth(startYear_);
        for (const auto &item : monthItems_) {
            billCount_ += item.billCount;
            totalRevenue_ += item.totalRevenue;
        }
    }
}

void RevenueStatisticsScreen::openPeriodMenu() {
    bool changed = selectPeriodRange(
        periodType_,
        startDate_, endDate_,
        startYear_, startMonth_,
        endYear_, endMonth_);

    if (changed) {
        reload();
    }
    draw();
}

void RevenueStatisticsScreen::drawDateData() {
    int n = (int)dateItems_.size();

    if (n > 0) {
        const int tableWidth = 41;
        printDivider('=', tableWidth);
        setColor(COLOR_ACCENT);
        stringstream hs;
        hs << left << setw(14) << "  Ngay"
           << left << setw(12) << "So hoa don"
           << left << setw(9) << "Doanh thu";
        cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
        resetColor();
        printDivider('-', tableWidth);

        for (int i = 0; i < n; ++i) {
            const RevenueByDate &item = dateItems_[i];
            stringstream ss;
            ss << "  " << left << setw(12) << item.date
               << left << setw(12) << item.billCount
               << right << setw(9) << (long long)item.totalRevenue;
            cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |\n";
        }

        printDivider('=', tableWidth);
    }
}

void RevenueStatisticsScreen::drawMonthData() {
    int n = (int)monthItems_.size();

    if (n > 0) {
        const int tableWidth = 44;
        printDivider('=', tableWidth);
        setColor(COLOR_ACCENT);
        stringstream hs;
        hs << left << setw(17) << "  Thang/Nam"
           << left << setw(12) << "So hoa don"
           << left << setw(9) << "Doanh thu";
        cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
        resetColor();
        printDivider('-', tableWidth);

        for (int i = 0; i < n; ++i) {
            const RevenueByMonth &item = monthItems_[i];
            stringstream ss;
            ss << "  " << left << setw(15) << ("Thang " + to_string(item.month) + "/" + to_string(item.year))
               << left << setw(12) << item.billCount
               << right << setw(9) << (long long)item.totalRevenue;
            cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |\n";
        }

        printDivider('=', tableWidth);
    }
}

void RevenueStatisticsScreen::draw() {
    cls();

    string subtitle;
    if (periodType_ == StatsPeriodType::DateRange) {
        subtitle = "Tu: " + startDate_ + " den: " + endDate_;
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        subtitle = "Tu: " + to_string(startMonth_) + "/" + to_string(startYear_) +
                   " den: " + to_string(endMonth_) + "/" + to_string(endYear_);
    } else if (periodType_ == StatsPeriodType::Year) {
        subtitle = "Nam: " + to_string(startYear_);
    }

    printTitleBox("THONG KE DOANH THU", subtitle);
    printDivider('=');

    cout << bullet(selectedIndex_ == 0) << "[Doi khoang thoi gian]\n\n";

    firstRow_ = contentRow();

    if (periodType_ == StatsPeriodType::DateRange) {
        drawDateData();
    } else {
        drawMonthData();
    }

    cout << "\n";
    setColor(COLOR_ACCENT);
    cout << "TONG KET:\n";
    resetColor();
    cout << "So hoa don      : " << billCount_ << "\n";
    cout << "Tong doanh thu  : " << (long long)totalRevenue_ << " VND\n";
    if (billCount_ > 0) {
        cout << "Trung binh/hoa don: " << (long long)(totalRevenue_ / billCount_) << " VND\n";
    }

    cout << "\n";
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_ << "\n\n";
        resetColor();
    }

    printHint("[Enter] doi thoi gian  |  [Backspace] quay lai");
    cout.flush();
}

void RevenueStatisticsScreen::onKey(Key k) {
    msg_.clear();

    if (k == KEY_ENTER) {
        openPeriodMenu();
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    }
}

// ======================= TopFoodsScreen =======================
TopFoodsScreen::TopFoodsScreen(App *a)
    : Screen(a),
      statisticsBLL_(a->statistics()),
      periodType_(StatsPeriodType::DateRange),
      startYear_(0), startMonth_(0),
      endYear_(0), endMonth_(0),
      topN_(10),
      selectedIndex_(-1),
      firstRow_(7),
      msg_("") {
    getDefaultDateRange(startDate_, endDate_);
    reload();
}

void TopFoodsScreen::reload() {
    items_.clear();

    if (periodType_ == StatsPeriodType::DateRange) {
        items_ = statisticsBLL_.getTopFoodsByRevenue(topN_, startDate_, endDate_);
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        char buffer[32];
        sprintf(buffer, "%04d-%02d-01", startYear_, startMonth_);
        string start = buffer;

        int lastDay = 31;
        if (endMonth_ == 2) {
            lastDay = (endYear_ % 4 == 0 && (endYear_ % 100 != 0 || endYear_ % 400 == 0)) ? 29 : 28;
        } else if (endMonth_ == 4 || endMonth_ == 6 || endMonth_ == 9 || endMonth_ == 11) {
            lastDay = 30;
        }
        sprintf(buffer, "%04d-%02d-%02d", endYear_, endMonth_, lastDay);
        string end = buffer;

        items_ = statisticsBLL_.getTopFoodsByRevenue(topN_, start, end);
    } else if (periodType_ == StatsPeriodType::Year) {
        char buffer[32];
        sprintf(buffer, "%04d-01-01", startYear_);
        string start = buffer;
        sprintf(buffer, "%04d-12-31", startYear_);
        string end = buffer;

        items_ = statisticsBLL_.getTopFoodsByRevenue(topN_, start, end);
    }

    if (selectedIndex_ >= (int)items_.size() && !items_.empty()) {
        selectedIndex_ = (int)items_.size() - 1;
    }
    if (selectedIndex_ < 0 && !items_.empty()) {
        selectedIndex_ = 0;
    }
}

void TopFoodsScreen::openPeriodMenu() {
    bool changed = selectPeriodRange(
        periodType_,
        startDate_, endDate_,
        startYear_, startMonth_,
        endYear_, endMonth_);

    if (changed) {
        reload();
    }
    draw();
}

void TopFoodsScreen::drawRow(int index, bool selected) {
    if (index < 0 || index >= (int)items_.size())
        return;

    const TopFood &item = items_[index];
    const int tableWidth = 61;
    int row = firstRow_ + index;

    gotoRC(row, 0);
    clearLine();

    stringstream ss;
    ss << "  " << left << setw(4) << (index + 1)
       << left << setw(30) << item.foodName
       << left << setw(10) << item.totalQuantity
       << right << setw(9) << (long long)item.totalRevenue;
    cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |";
    cout.flush();
}
void TopFoodsScreen::draw() {
    cls();

    string subtitle;
    if (periodType_ == StatsPeriodType::DateRange) {
        subtitle = "Tu: " + startDate_ + " den: " + endDate_;
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        subtitle = "Tu: " + to_string(startMonth_) + "/" + to_string(startYear_) +
                   " den: " + to_string(endMonth_) + "/" + to_string(endYear_);
    } else if (periodType_ == StatsPeriodType::Year) {
        subtitle = "Nam: " + to_string(startYear_);
    }

    printTitleBox("TOP 10 MON AN BAN CHAY", subtitle);
    printDivider('=');
    cout << "> [Doi khoang thoi gian]\n\n";
    printDivider('=');

    int n = (int)items_.size();
    if (n > 0) {
        const int tableWidth = 61;
        printDivider('=', tableWidth);
        setColor(COLOR_ACCENT);
        stringstream hs;
        hs << left << setw(6) << "  #"
           << left << setw(30) << "Ten mon"
           << left << setw(10) << "So luong"
           << left << setw(9) << "Doanh thu";
        cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
        resetColor();
        printDivider('-', tableWidth);

        firstRow_ = contentRow();

        for (int i = 0; i < n; ++i) {
            drawRow(i, false);
            cout << "\n";
        }

        printDivider('=', tableWidth);
    } else {
        setColor(COLOR_MUTED);
        cout << "(Khong co du lieu mon an trong khoang thoi gian nay)\n";
        cout << "Luu y: Can co chi tiet hoa don (bill_items) de hien thi thong ke mon an.\n";
        resetColor();
    }

    cout << "\n";
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_ << "\n\n";
        resetColor();
    }

    printHint("[Enter] doi thoi gian  |  [Backspace] quay lai");
    cout.flush();
}

void TopFoodsScreen::onKey(Key k) {
    msg_.clear();

    if (k == KEY_ENTER) {
        openPeriodMenu();
        return;
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    }
} // ======================= TransactionHistoryScreen =======================
TransactionHistoryScreen::TransactionHistoryScreen(App *a)
    : Screen(a),
      statisticsBLL_(a->statistics()),
      periodType_(StatsPeriodType::DateRange),
      startYear_(0), startMonth_(0),
      endYear_(0), endMonth_(0),
      currentPage_(1),
      totalPages_(1),
      topActionRow_(3),
      topActionIndex_(0),
      onTopAction_(true),
      selectedIndex_(0),
      firstRow_(7),
      msg_("") {
    getDefaultDateRange(startDate_, endDate_);
    reload();
}

void TransactionHistoryScreen::reload() {
    allItems_.clear();

    if (periodType_ == StatsPeriodType::DateRange) {
        allItems_ = statisticsBLL_.getTransactionHistory(startDate_, endDate_);
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        char buffer[32];
        sprintf(buffer, "%04d-%02d-01", startYear_, startMonth_);
        string start = buffer;

        int lastDay = 31;
        if (endMonth_ == 2) {
            lastDay = (endYear_ % 4 == 0 && (endYear_ % 100 != 0 || endYear_ % 400 == 0)) ? 29 : 28;
        } else if (endMonth_ == 4 || endMonth_ == 6 || endMonth_ == 9 || endMonth_ == 11) {
            lastDay = 30;
        }
        sprintf(buffer, "%04d-%02d-%02d", endYear_, endMonth_, lastDay);
        string end = buffer;

        allItems_ = statisticsBLL_.getTransactionHistory(start, end);
    } else if (periodType_ == StatsPeriodType::Year) {
        char buffer[32];
        sprintf(buffer, "%04d-01-01", startYear_);
        string start = buffer;
        sprintf(buffer, "%04d-12-31", startYear_);
        string end = buffer;

        allItems_ = statisticsBLL_.getTransactionHistory(start, end);
    }

    totalPages_ = ((int)allItems_.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (totalPages_ < 1)
        totalPages_ = 1;

    currentPage_ = 1;
    loadPage();
}

void TransactionHistoryScreen::loadPage() {
    pageItems_.clear();

    int start = (currentPage_ - 1) * ITEMS_PER_PAGE;
    int end = min(start + ITEMS_PER_PAGE, (int)allItems_.size());

    for (int i = start; i < end; ++i) {
        pageItems_.push_back(allItems_[i]);
    }

    if (selectedIndex_ >= (int)pageItems_.size() && !pageItems_.empty()) {
        selectedIndex_ = (int)pageItems_.size() - 1;
    }
    if (selectedIndex_ < 0 && !pageItems_.empty()) {
        selectedIndex_ = 0;
    }
}

void TransactionHistoryScreen::openPeriodMenu() {
    bool changed = selectPeriodRange(
        periodType_,
        startDate_, endDate_,
        startYear_, startMonth_,
        endYear_, endMonth_);

    if (changed) {
        reload();
    }
    draw();
}

void TransactionHistoryScreen::drawTopActions() {
    gotoRC(topActionRow_, 0);
    clearLine();

    bool onPeriodMenu = onTopAction_ && topActionIndex_ == 0;
    cout << bullet(onPeriodMenu) << "[Doi khoang thoi gian]";

    if (totalPages_ > 1) {
        cout << "   ";

        bool hasPrev = currentPage_ > 1;
        bool hasNext = currentPage_ < totalPages_;
        bool onPrev = onTopAction_ && topActionIndex_ == 1;
        bool onNext = onTopAction_ && topActionIndex_ == 2;

        cout << bullet(onPrev) << "[" << (hasPrev ? "Trang truoc" : "---") << "]";
        cout << "  ";
        cout << bullet(onNext) << "[" << (hasNext ? "Trang sau" : "---") << "]";
    }

    cout << "\n\n";
    cout.flush();
}

void TransactionHistoryScreen::draw() {
    cls();

    string subtitle;
    if (periodType_ == StatsPeriodType::DateRange) {
        subtitle = "Tu: " + startDate_ + " den: " + endDate_;
    } else if (periodType_ == StatsPeriodType::MonthRange) {
        subtitle = "Tu: " + to_string(startMonth_) + "/" + to_string(startYear_) +
                   " den: " + to_string(endMonth_) + "/" + to_string(endYear_);
    } else if (periodType_ == StatsPeriodType::Year) {
        subtitle = "Nam: " + to_string(startYear_);
    }

    printTitleBox("LICH SU GIAO DICH", subtitle);
    printDivider('=');

    topActionRow_ = contentRow();
    drawTopActions();

    int n = (int)pageItems_.size();

    if (n > 0) {
        const int tableWidth = 55;
        printDivider('=', tableWidth);
        setColor(COLOR_ACCENT);
        stringstream hs;
        hs << left << setw(6) << "  ID"
           << left << setw(5) << "Ban"
           << left << setw(11) << "Tong tien"
           << left << setw(21) << "Ngay thanh toan"
           << right << setw(6) << "So mon";
        cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
        resetColor();
        printDivider('-', tableWidth);

        firstRow_ = contentRow();

        for (int i = 0; i < n; ++i) {
            const TransactionHistory &item = pageItems_[i];

            string paid = item.paidDate;
            size_t dotPos = paid.find('.');
            if (dotPos != string::npos) {
                paid = paid.substr(0, dotPos);
            }

            stringstream ss;
            ss << "  "
               << left << setw(4) << item.billId
               << left << setw(5) << item.tableId
               << right << setw(9) << (long long)item.totalPrice << "  "
               << left << setw(21) << paid
               << left << setw(6) << item.itemCount;
            cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |\n";
        }

        printDivider('=', tableWidth);

        // Thong ke trang hien tai
        double pageTotal = 0;
        for (const auto &item : pageItems_) {
            pageTotal += item.totalPrice;
        }

        setColor(COLOR_ACCENT);
        cout << "Trang " << currentPage_ << "/" << totalPages_
             << " | Tong trang nay: " << n << " giao dich | "
             << (long long)pageTotal << " VND\n";
        resetColor();

    } else {
        setColor(COLOR_MUTED);
        cout << "(Khong co giao dich)\n";
        resetColor();
    }

    cout << "\n";
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_ << "\n\n";
        resetColor();
    }

    printHint(totalPages_ > 1 ? "[Left/Right] doi khoang/phan trang  |  [Enter] thuc hien  |  [Backspace] quay lai"
                              : "[Enter] doi khoang thoi gian  |  [Backspace] quay lai");

    cout.flush();
}

void TransactionHistoryScreen::onKey(Key k) {
    int oldTopIndex = topActionIndex_;

    int maxTopIndex = (totalPages_ > 1) ? 2 : 0;

    if (k == KEY_LEFT) {
        if (topActionIndex_ > 0)
            topActionIndex_--;
    } else if (k == KEY_RIGHT) {
        if (topActionIndex_ < maxTopIndex)
            topActionIndex_++;
    } else if (k == KEY_ENTER) {
        if (topActionIndex_ == 0) {
            // Doi khoang thoi gian
            openPeriodMenu();
            return;
        } else if (topActionIndex_ == 1) {
            // Trang truoc
            if (currentPage_ > 1) {
                currentPage_--;
                loadPage();
                draw();
            }
            return;
        } else if (topActionIndex_ == 2) {
            // Trang sau
            if (currentPage_ < totalPages_) {
                currentPage_++;
                loadPage();
                draw();
            }
            return;
        }
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    }

    if (oldTopIndex != topActionIndex_) {
        drawTopActions();
    }
}
