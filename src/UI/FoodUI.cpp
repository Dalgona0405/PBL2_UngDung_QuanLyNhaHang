// FoodUI.cpp
#include "FoodUI.h"
#include "BillUI.h"
#include "CategoryUI.h"
#include "ConsoleUtils.h"
#include "SearchSortUtils.h"
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

// ======================= FoodListScreen =======================
FoodListScreen::FoodListScreen(App *a, bool selectMode, BillItemEditScreen *billItemScreen)
    : PagedListScreen<Food>(a),
      foodBLL_(a->food()),
      catBLL_(a->category()),
      selectMode_(selectMode),
      billItemScreen_(billItemScreen),
      inAction_(false),
      actionIndex_(0),
      msg_(""),
      showingMsg_(false),
      viewNote_(""),
      filterType_(FoodFilterType::None),
      sortField_(FoodSortField::None),
      sortAsc_(true),
      keyword_(""),
      filterCategory_(""),
      minPrice_(0),
      maxPrice_(0),
      topActionRow_(3),
      topActionIndex_(0),
      onTopAction_(true),
      msgRow_(0) {
    reloadData();
}

string FoodListScreen::buildFilterLabel() const {
    if (filterType_ == FoodFilterType::Name) {
        return string("Ten chua \"") + keyword_ + "\"";
    }
    if (filterType_ == FoodFilterType::Category) {
        return string("Danh muc \"") + filterCategory_ + "\"";
    }
    if (filterType_ == FoodFilterType::PriceRange) {
        stringstream ss;
        ss << "Gia tu " << (long long)minPrice_ << " den " << (long long)maxPrice_;
        return ss.str();
    }
    return string();
}

string FoodListScreen::buildSortLabel() const {
    if (sortField_ == FoodSortField::Name) {
        return string("Sap xep ten (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
    }
    if (sortField_ == FoodSortField::Price) {
        return string("Sap xep gia (") + (sortAsc_ ? "thap->cao" : "cao->thap") + ")";
    }
    if (sortField_ == FoodSortField::Category) {
        return string("Sap xep danh muc (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
    }
    return string();
}

void FoodListScreen::updateViewNoteFromState() {
    string note = buildFilterLabel();
    string sortLabel = buildSortLabel();
    if (!sortLabel.empty()) {
        if (!note.empty())
            note += " | " + sortLabel;
        else
            note = sortLabel;
    }
    viewNote_ = note;
}

void FoodListScreen::drawOptionRow(short startRow, const vector<string> &options, int index, bool selected) const {
    gotoRC(startRow + index, 0);
    clearLine();
    cout << bullet(selected) << options[index];
}

void FoodListScreen::drawMenuOptions(short startRow, const vector<string> &options, int sel) const {
    for (size_t i = 0; i < options.size(); ++i) {
        drawOptionRow(startRow, options, (int)i, (int)i == sel);
    }
}

bool FoodListScreen::promptInlineString(short row, const string &prefix, string &value) {
    return ::promptInlineText(row, prefix, value);
}

bool FoodListScreen::promptInlineInt(short row, const string &prefix, int &value) {
    return ::promptInlineInt(row, prefix, value);
}

bool FoodListScreen::promptInlineDouble(short row, const string &prefix, double &value) {
    return ::promptInlineDouble(row, prefix, value);
}

void FoodListScreen::reloadData() {
    if (filterType_ == FoodFilterType::Name) {
        items_ = foodBLL_.searchByName(keyword_);
    } else if (filterType_ == FoodFilterType::Category) {
        items_ = foodBLL_.searchByCategory(filterCategory_);
    } else if (filterType_ == FoodFilterType::PriceRange) {
        items_ = foodBLL_.searchByPriceRange(minPrice_, maxPrice_);
    } else {
        items_ = foodBLL_.getAll();
    }

    applySort();
    calculatePagination();
}

string FoodListScreen::categoryNameOf(const Food &f) const {
    Category cat;
    if (catBLL_.getById(f.categoryId, cat))
        return cat.name;
    return string("N/A");
}

void FoodListScreen::applySort() {
    if (sortField_ == FoodSortField::None)
        return;

    if (filterType_ == FoodFilterType::None) {
        if (sortField_ == FoodSortField::Name) {
            items_ = foodBLL_.getAllSortedByName(sortAsc_);
        } else if (sortField_ == FoodSortField::Price) {
            items_ = foodBLL_.getAllSortedByPrice(sortAsc_);
        } else if (sortField_ == FoodSortField::Category) {
            items_ = foodBLL_.getAllSortedByCategory(sortAsc_);
        }
        return;
    }

    sort(items_.begin(), items_.end(), [&](const Food &a, const Food &b) {
        if (sortField_ == FoodSortField::Name) {
            return sortAsc_ ? a.name < b.name : a.name > b.name;
        }
        if (sortField_ == FoodSortField::Price) {
            return sortAsc_ ? a.price < b.price : a.price > b.price;
        }
        if (sortField_ == FoodSortField::Category) {
            string ca = a.categoryName;
            string cb = b.categoryName;
            return sortAsc_ ? ca < cb : ca > cb;
        }
        return false;
    });
}

void FoodListScreen::resetView() {
    filterType_ = FoodFilterType::None;
    sortField_ = FoodSortField::None;
    sortAsc_ = true;
    keyword_.clear();
    filterCategory_.clear();
    minPrice_ = 0;
    maxPrice_ = 0;
    viewNote_.clear();
}

bool FoodListScreen::promptStringInput(const string &title, const string &label, string &out) {
    string buffer = out;
    cls();
    printTitleBox(title);
    printDivider('=');
    short inputRow = contentRow();
    cout << label << buffer << "\n\n";
    printHint("[Enter] xac nhan  |  [Esc] huy");
    showCursor();

    while (true) {
        gotoRC(inputRow, 0);
        clearLine();
        cout << label << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            out = buffer;
            hideCursor();
            return true;
        } else if (ch == 27) { // ESC -> cancel
            hideCursor();
            return false;
        } else if (ch == 8) {
            if (!buffer.empty())
                buffer.pop_back();
        } else if (isprint(static_cast<unsigned char>(ch))) {
            buffer.push_back(static_cast<char>(ch));
        }
    }
}

bool FoodListScreen::promptIntInput(const string &title, const string &label, int &out) {
    string buffer;
    if (out != 0)
        buffer = to_string(out);

    cls();
    printTitleBox(title);
    printDivider('=');
    short inputRow = contentRow();
    cout << label << buffer << "\n\n";
    printHint("[Enter] xac nhan  |  [Esc] huy");
    showCursor();

    while (true) {
        gotoRC(inputRow, 0);
        clearLine();
        cout << label << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) {
            out = buffer.empty() ? 0 : atoi(buffer.c_str());
            hideCursor();
            return true;
        } else if (ch == 27) {
            hideCursor();
            return false;
        } else if (ch == 8) {
            if (!buffer.empty())
                buffer.pop_back();
        } else if (isdigit(static_cast<unsigned char>(ch))) {
            buffer.push_back(static_cast<char>(ch));
        }
    }
}

bool FoodListScreen::promptDoubleInput(const string &title, const string &label, double &out) {
    string buffer;
    if (out != 0) {
        stringstream ss;
        ss << fixed << setprecision(0) << out;
        buffer = ss.str();
    }

    cls();
    printTitleBox(title);
    printDivider('=');
    short inputRow = contentRow();
    cout << label << buffer << "\n\n";
    printHint("[Enter] xac nhan  |  [Esc] huy");
    showCursor();

    while (true) {
        gotoRC(inputRow, 0);
        clearLine();
        cout << label << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) {
            try {
                out = buffer.empty() ? 0 : stod(buffer);
            } catch (...) {
                out = 0;
            }
            hideCursor();
            return true;
        } else if (ch == 27) {
            hideCursor();
            return false;
        } else if (ch == 8) {
            if (!buffer.empty())
                buffer.pop_back();
        } else if (isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
            if (ch == '.' && buffer.find('.') != string::npos)
                continue;
            buffer.push_back(static_cast<char>(ch));
        }
    }
}

bool FoodListScreen::promptPriceRange(double &minPrice, double &maxPrice) {
    double minP = minPrice;
    double maxP = maxPrice;
    if (!promptDoubleInput("Tim kiem theo gia", "Gia tu   : ", minP))
        return false;
    if (!promptDoubleInput("Tim kiem theo gia", "Gia den  : ", maxP))
        return false;
    if (minP > maxP)
        swap(minP, maxP);
    minPrice = minP;
    maxPrice = maxP;
    return true;
}

bool FoodListScreen::promptSortOrder(bool &asc) {
    cls();
    printTitleBox("Chon thu tu sap xep");
    printDivider('=');
    vector<string> options = {"Tang dan", "Giam dan"};
    int sel = asc ? 0 : 1;
    short startRow = contentRow();

    drawMenuOptions(startRow, options, sel);
    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] xac nhan  |  [Backspace]/[Esc] huy");
    cout.flush();

    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC) {
            return false;
        } else if (k == KEY_UP && sel > 0) {
            sel--;
            drawMenuOptions(startRow, options, sel);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            sel++;
            drawMenuOptions(startRow, options, sel);
        } else if (k == KEY_ENTER) {
            asc = (sel == 0);
            return true;
        }
    }
}

vector<FoodListScreen::FoodAction> FoodListScreen::topActions() const {
    vector<FoodAction> acts;
    if (!selectMode_)
        acts.push_back(FoodAction::Add);
    acts.push_back(FoodAction::Search);
    acts.push_back(FoodAction::Sort);
    return acts;
}

void FoodListScreen::drawTopActions() {
    vector<FoodAction> actions = topActions();
    if (topActionIndex_ >= (int)actions.size())
        topActionIndex_ = (int)actions.size() - 1;

    gotoRC(topActionRow_, 0);
    clearLine();

    for (size_t i = 0; i < actions.size(); ++i) {
        if (i > 0)
            cout << "   ";
        bool selected = onTopAction_ && (int)i == topActionIndex_;
        cout << bullet(selected);
        switch (actions[i]) {
        case FoodAction::Add:
            cout << "[Them]";
            break;
        case FoodAction::Search:
            cout << "[Tim kiem]";
            break;
        case FoodAction::Sort:
            cout << "[Sap xep]";
            break;
        }
    }
    cout.flush();
}

void FoodListScreen::handleTopAction(FoodAction action) {
    if (action == FoodAction::Add) {
        Food initial;
        initial.id = 0;
        initial.name = "";
        initial.categoryId = 0;
        initial.price = 0;
        app->push(new FoodEditScreen(app, true, initial));
        return;
    }
    if (action == FoodAction::Search) {
        openSearchMenu();
        return;
    }
    openSortMenu();
}
void FoodListScreen::openSearchMenu() {
    inAction_ = false;
    msg_.clear();

    vector<string> options = {
        "Hien tat ca",
        "Tim theo ten",
        "Tim theo danh muc",
        "Tim theo gia (min-max)"};

    int sel = 0;
    cls();
    printTitleBox("TIM KIEM MON AN");
    printDivider('=');
    cout << "Chon cach tim:\n";

    short menuStartRow = contentRow();
    auto redrawItem = [&](int index, bool selected) {
        gotoRC(menuStartRow + index, 0);
        clearLine();
        cout << bullet(selected) << options[index];
    };

    for (size_t i = 0; i < options.size(); ++i) {
        cout << bullet((int)i == sel) << options[i] << "\n";
    }

    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] thuc hien  |  [Backspace]/[Esc] huy");
    cout.flush();

    auto redrawMenu = [&]() {
        for (size_t i = 0; i < options.size(); ++i) {
            redrawItem((int)i, (int)i == sel);
        }
    };

    bool changed = false;
    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC) {
            break;
        } else if (k == KEY_UP && sel > 0) {
            redrawItem(sel, false);
            sel--;
            redrawItem(sel, true);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            redrawItem(sel, false);
            sel++;
            redrawItem(sel, true);
        } else if (k == KEY_ENTER) {
            if (sel == 0) {
                resetView();
                updateViewNoteFromState();
                changed = true;
                break;
            } else if (sel == 1) {
                string kw = keyword_;
                int result = ::promptInlineText(menuStartRow + sel, options[sel], kw);
                if (result == 1) {
                    filterType_ = FoodFilterType::Name;
                    keyword_ = kw;
                    changed = true;
                    updateViewNoteFromState();
                    break;
                } else if (result == 0) {
                    redrawItem(sel, true);
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 2) {
                string catName = filterCategory_;
                int result = ::promptInlineText(menuStartRow + sel, options[sel], catName);
                if (result == 1) {
                    filterType_ = FoodFilterType::Category;
                    filterCategory_ = catName;
                    changed = true;
                    updateViewNoteFromState();
                    break;
                } else if (result == 0) {
                    redrawItem(sel, true);
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 3) {
                double minP = minPrice_;
                double maxP = maxPrice_;
                int r1 = ::promptInlineDouble(menuStartRow + sel, options[sel] + " : Gia tu", minP);
                if (r1 == -1)
                    break;
                if (r1 == 0) {
                    redrawItem(sel, true);
                } else if (r1 == 1) {
                    string label2 = options[sel] + " : Gia tu: " + to_string((long long)minP) + "   Gia den";
                    int r2 = ::promptInlineDouble(menuStartRow + sel, label2, maxP);
                    if (r2 == -1)
                        break;
                    if (r2 == 0) {
                        redrawItem(sel, true);
                    } else if (r2 == 1) {
                        if (minP > maxP)
                            swap(minP, maxP);
                        filterType_ = FoodFilterType::PriceRange;
                        minPrice_ = minP;
                        maxPrice_ = maxP;
                        changed = true;
                        updateViewNoteFromState();
                        break;
                    }
                }
            }
        } else {
            redrawMenu();
        }
    }

    if (changed)
        reloadData();
    onTopAction_ = true;
    draw();
}

void FoodListScreen::openSortMenu() {
    inAction_ = false;
    msg_.clear();

    vector<pair<string, FoodSortField>> options = {
        //{"Bo sap xep", FoodSortField::None},
        {"Theo ten", FoodSortField::Name},
        {"Theo gia", FoodSortField::Price},
        {"Theo danh muc", FoodSortField::Category}};

    int sel = 0;
    bool asc = sortAsc_;
    for (size_t i = 0; i < options.size(); ++i) {
        if (sortField_ == options[i].second) {
            sel = (int)i;
            break;
        }
    }

    cls();
    printTitleBox("SAP XEP MON AN");
    printDivider('=');
    cout << "Chon truong sap xep:\n";

    short menuStartRow = contentRow();
    auto redrawItem = [&](int index, bool selected) {
        gotoRC(menuStartRow + index, 0);
        clearLine();
        cout << bullet(selected) << options[index].first;
    };

    for (size_t i = 0; i < options.size(); ++i) {
        cout << bullet((int)i == sel) << options[i].first << "\n";
    }

    cout << "\n";
    printHint("[Up/Down] chon truong  |  [Enter] chon  |  [Backspace]/[Esc] huy");
    cout.flush();

    auto redrawMenu = [&]() {
        for (size_t i = 0; i < options.size(); ++i) {
            redrawItem((int)i, (int)i == sel);
        }
    };

    bool changed = false;
    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC) {
            break;
        } else if (k == KEY_UP && sel > 0) {
            redrawItem(sel, false);
            sel--;
            redrawItem(sel, true);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            redrawItem(sel, false);
            sel++;
            redrawItem(sel, true);
        } else if (k == KEY_ENTER) {
            FoodSortField chosen = options[sel].second;
            if (chosen == FoodSortField::None) {
                sortField_ = FoodSortField::None;
                sortAsc_ = true;
                updateViewNoteFromState();
                changed = true;
            } else {
                short row = menuStartRow + sel;
                bool localAsc = asc;
                while (true) {
                    gotoRC(row, 0);
                    clearLine();
                    cout << bullet(true) << options[sel].first << " : "
                         << (localAsc ? "[*Tang dan*] " : "[Tang dan] ")
                         << (!localAsc ? "[*Giam dan*]" : "[Giam dan]");
                    cout.flush();
                    Key k2 = readKey();
                    if (k2 == KEY_LEFT) {
                        localAsc = true;
                    } else if (k2 == KEY_RIGHT) {
                        localAsc = false;
                    } else if (k2 == KEY_ENTER) {
                        sortField_ = chosen;
                        sortAsc_ = localAsc;
                        updateViewNoteFromState();
                        changed = true;
                        goto exit_sort_menu;
                        // break;
                    } else if (k2 == KEY_BACKSPACE) {
                        break;
                    } else if (k2 == KEY_ESC) {
                        changed = false;
                        goto exit_sort_menu;
                    }
                }
                redrawItem(sel, true);
            }
        } else {
            redrawMenu();
        }
    }

exit_sort_menu:
    if (changed) {
        reloadData();
    }
    onTopAction_ = true;
    draw();
}

void FoodListScreen::drawTableHeader() {
    const int tableWidth = 84;
    printDivider('=', tableWidth);
    setColor(COLOR_ACCENT);
    stringstream hs;
    hs << "  "
       << left << setw(6) << " ID"
       << left << setw(30) << "Ten mon"
       << left << setw(30) << "Danh muc"
       << right << setw(12) << "Gia";
    cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
    resetColor();
    printDivider('-', tableWidth);
}

void FoodListScreen::drawRowItem(int index, const Food &item, bool selected) {
    const int tableWidth = 84;
    int relativeRow = firstRow_ + (index % ITEMS_PER_PAGE);
    gotoRC(relativeRow, 0);
    clearLine();

    bool showArrow = (selected && !inAction_ && !onTopAction_);

    stringstream ss;
    ss << bullet(showArrow)
       << left << setw(6) << item.id
       << left << setw(30) << item.name
       << left << setw(30) << categoryNameOf(item)
       << right << setw(12) << (int)item.price;
    string line = ss.str();
    cout << "| " << left << setw(tableWidth - 4) << line << " |";

    if (!selectMode_ && selected && inAction_) {
        cout << "   ";
        setColor(COLOR_ACCENT);
        cout << ((actionIndex_ == 0) ? "[*Sua*]" : "[Sua]");
        resetColor();
        cout << " ";
        setColor(COLOR_ALERT);
        cout << ((actionIndex_ == 1) ? "[*Xoa*]" : "[Xoa]");
        resetColor();
    }
}

void FoodListScreen::draw() {
    reloadData();
    cls();

    string title = selectMode_ ? "CHON MON AN" : "QUAN LY MON AN";
    string subtitle = selectMode_ ? "Len/Xuong de chon, Enter de them vao hoa don" : "Enter de Them hoac mo menu Sua/Xoa";
    printTitleBox(title, subtitle);
    printDivider('=');
    if (!viewNote_.empty()) {
        setColor(COLOR_MUTED);
        cout << viewNote_ << "\n\n";
        resetColor();
    }

    topActionRow_ = contentRow();
    drawTopActions();
    cout << "\n";
    drawList();

    cout << "\n";
    if (selectMode_) {
        printHint("[Up/Down] chon mon  |  [Left/Right] trang/chuc nang  |  [Enter] thuc hien  |  [Backspace] quay lai");
    } else {
        if (!inAction_) {
            printHint("[Up/Down] chon mon  |  [Left/Right] trang/chuc nang  |  [Enter] thuc hien  |  [Backspace] quay lai");
        } else {
            printHint("[Left/Right] chon Sua/Xoa  |  [Enter] thuc hien  |  [Backspace] huy menu");
        }
    }

    msgRow_ = contentRow();
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_;
        resetColor();
    }
    cout.flush();
}

void FoodListScreen::onKey(Key k) {
    if (showingMsg_) {
        showingMsg_ = false;
        msg_.clear();
        gotoRC(msgRow_, 0);
        clearLine();
        cout.flush();
    }

    if (onTopAction_) {
        vector<FoodAction> actions = topActions();
        int oldTopIndex = topActionIndex_;
        if (k == KEY_LEFT) {
            msg_.clear();
            if (topActionIndex_ > 0)
                topActionIndex_--;
        } else if (k == KEY_RIGHT) {
            msg_.clear();
            if (topActionIndex_ + 1 < (int)actions.size())
                topActionIndex_++;
        } else if (k == KEY_DOWN) {
            msg_.clear();
            onTopAction_ = false;
            if (items_.empty())
                onTopAction_ = true;
            if (!onTopAction_) {
                drawTopActions();
                redrawRow(selectedIndex_, true);
                cout.flush();
            }
            return;
        } else if (k == KEY_ENTER) {
            handleTopAction(actions[topActionIndex_]);
            return;
        } else if (k == KEY_BACKSPACE) {
            app->pop();
            return;
        }
        if (oldTopIndex != topActionIndex_) {
            drawTopActions();
        }
        return;
    } else if (!inAction_) {
        int navResult = handleNavigationKey(k);
        if (navResult == 2) {
            msg_.clear();
            draw();
            return;
        }
        if (navResult == 1) {
            msg_.clear();
            cout.flush();
            return;
        }
        if (k == KEY_UP) {
            if (selectedIndex_ == 0 && currentPage_ == 1) {
                msg_.clear();
                onTopAction_ = true;
                redrawRow(selectedIndex_, false);
                drawTopActions();
                return;
            }
        } else if (k == KEY_ENTER) {
            msg_.clear();
            if (selectMode_) {
                if (billItemScreen_ != NULL && selectedIndex_ >= 0) {
                    billItemScreen_->setFood(items_[selectedIndex_]);
                    app->pop();
                }
            } else {
                if (selectedIndex_ >= 0) {
                    inAction_ = true;
                    actionIndex_ = 0;
                    redrawRow(selectedIndex_, true);
                    cout.flush();
                }
            }
        } else if (k == KEY_BACKSPACE) {
            app->pop();
            return;
        }
        return;
    } else {
        int oldAction = actionIndex_;
        if (k == KEY_LEFT) {
            if (actionIndex_ > 0)
                actionIndex_--;
        } else if (k == KEY_RIGHT) {
            if (actionIndex_ < 1)
                actionIndex_++;
        } else if (k == KEY_BACKSPACE) {
            inAction_ = false;
            msg_.clear();
            redrawRow(selectedIndex_, true);
            cout.flush();
            return;
        } else if (k == KEY_ENTER) {
            if (selectedIndex_ >= 0 && selectedIndex_ < (int)items_.size()) {
                Food f = items_[selectedIndex_];
                if (actionIndex_ == 0) {
                    app->push(new FoodEditScreen(app, false, f));
                    inAction_ = false; //
                } else {
                    Result r = foodBLL_.removeChecked(f.id);
                    msg_ = r.message;
                    if (r.ok) {
                        showingMsg_ = true;
                        reloadData();
                        inAction_ = false;
                        if (selectedIndex_ >= (int)items_.size())
                            selectedIndex_ = (int)items_.size() - 1;
                        if (selectedIndex_ < 0 && !items_.empty())
                            selectedIndex_ = 0;
                        draw();
                    } else {
                        showingMsg_ = true;
                        redrawRow(selectedIndex_, true);
                        cout.flush();
                    }
                }
            }
        }

        if (inAction_ && oldAction != actionIndex_) {
            redrawRow(selectedIndex_, true);
            cout.flush();
        }
    }
}

// ======================= FoodEditScreen =======================
FoodEditScreen::FoodEditScreen(App *a, bool isNew, const Food &initial)
    : Screen(a),
      foodBLL_(a->food()),
      catBLL_(a->category()),
      isNew_(isNew),
      food_(initial),
      focus_(0),
      msg_("") {}

string FoodEditScreen::categoryName(int categoryId) const {
    if (categoryId == 0)
        return string("(chua chon)");

    Category cat;
    if (catBLL_.getById(categoryId, cat))
        return cat.name;
    return string("N/A");
}

void FoodEditScreen::drawForm() const {
    cls();
    gotoRC(0, 0);

    cout << (isNew_ ? "=== THEM MON AN ===\n\n" : "=== SUA MON AN ===\n\n");

    cout << (focus_ == 0 ? "> " : "  ");
    cout << "Ten mon an: " << food_.name << "\n";

    cout << (focus_ == 1 ? "> " : "  ");
    cout << "Danh muc  : " << categoryName(food_.categoryId) << "\n";

    cout << (focus_ == 2 ? "> " : "  ");
    cout << "Gia tien  : " << (int)food_.price << "\n\n";

    cout << (focus_ == 3 ? "> " : "  ");
    cout << "[Xac nhan]\n\n";

    if (!msg_.empty()) {
        cout << msg_ << "\n\n";
    }

    gotoRC(20, 0);
    clearLine();
    printHint("[Up/Down] chon field  |  [Enter] edit/chap nhan  |  [Backspace] quay lai (huy)  |  [Esc] thoat");
    cout.flush();
}

void FoodEditScreen::draw() {
    drawForm();
}

void FoodEditScreen::redrawFocusLines() {
    gotoRC(2, 0);
    clearLine();
    cout << (focus_ == 0 ? "> " : "  ")
         << "Ten mon an: " << food_.name;

    gotoRC(3, 0);
    clearLine();
    cout << (focus_ == 1 ? "> " : "  ")
         << "Danh muc  : " << categoryName(food_.categoryId);

    gotoRC(4, 0);
    clearLine();
    cout << (focus_ == 2 ? "> " : "  ")
         << "Gia tien  : " << (int)food_.price;

    gotoRC(6, 0);
    clearLine();
    cout << (focus_ == 3 ? "> " : "  ")
         << "[Xac nhan]";

    cout.flush();
}

void FoodEditScreen::showMsg() {
    gotoRC(10, 0);
    clearLine();
    if (!msg_.empty())
        cout << msg_;
    cout.flush();
}

void FoodEditScreen::editName(Key k) {
    string buffer = food_.name;
    showCursor();

    while (true) {
        gotoRC(2, 0);
        clearLine();
        cout << (focus_ == 0 ? "> " : "  ")
             << "Ten mon an: " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch(); // swallow arrow/function keys
            continue;
        }
        if (ch == 13) { // Enter
            food_.name = buffer;
            break;
        } else if (ch == 27) { // ESC
            break;
        } else if (ch == 8) { // Backspace
            if (!buffer.empty())
                buffer.erase(buffer.size() - 1);
        } else {
            if (isprint(static_cast<unsigned char>(ch)))
                buffer.push_back(static_cast<char>(ch));
        }
    }

    redrawFocusLines();
    hideCursor();
}

void FoodEditScreen::editPrice(Key k) {
    string buffer;
    if (food_.price > 0) {
        char tmp[32];
        sprintf(tmp, "%.0f", food_.price);
        buffer = tmp;
    }
    showCursor();

    while (true) {
        gotoRC(4, 0);
        clearLine();
        cout << (focus_ == 2 ? "> " : "  ")
             << "Gia tien  : " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            try {
                double price = stod(buffer);
                food_.price = price;
            } catch (...) {
                food_.price = 0;
            }
            break;
        } else if (ch == 27) { // ESC
            break;
        } else if (ch == 8) { // Backspace
            if (!buffer.empty())
                buffer.erase(buffer.size() - 1);
        } else {
            if (isprint(static_cast<unsigned char>(ch)))
                buffer.push_back(static_cast<char>(ch));
        }
    }

    redrawFocusLines();
    hideCursor();
}

void FoodEditScreen::chooseCategory() {
    msg_ = " ";
    showMsg();
    app->push(new CategoryListScreen(app, true, this));
}

void FoodEditScreen::save() {
    Result r;
    if (isNew_) {
        r = foodBLL_.addChecked(food_);
    } else {
        r = foodBLL_.updateChecked(food_);
    }

    msg_ = r.message;
    showMsg();

    gotoRC(20, 0);
    clearLine();
    printHint("[Up/Down] chon field  |  [Enter] edit/chap nhan  |  [Backspace] quay lai (huy)  |  [Esc] thoat");
    cout.flush();
}

void FoodEditScreen::onKey(Key k) {
    bool focusChanged = false;

    if (k == KEY_UP && focus_ > 0) {
        focus_--;
        focusChanged = true;
    } else if (k == KEY_DOWN && focus_ < 3) {
        focus_++;
        focusChanged = true;
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    } else if (k == KEY_ENTER) {
        if (focus_ == 0) {
            editName(k);
        } else if (focus_ == 1) {
            chooseCategory();
            return;
        } else if (focus_ == 2) {
            editPrice(k);
        } else if (focus_ == 3) {
            save();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}

void FoodEditScreen::setCategory(int categoryId) {
    food_.categoryId = categoryId;
    redrawFocusLines();
}
