// CategoryUI.cpp
#include "CategoryUI.h"
#include "ConsoleUtils.h"
#include "FoodUI.h"
#include "SearchSortUtils.h"
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// ======================= CategoryListScreen =======================
CategoryListScreen::CategoryListScreen(App *a, bool selectMode, FoodEditScreen *foodScreen)
    : PagedListScreen<Category>(a),
      catBLL_(a->category()),
      selectMode_(selectMode),
      foodScreen_(foodScreen),
      inAction_(false),
      actionIndex_(0),
      msg_(""),
      showingMsg_(false),
      viewNote_(""),
      msgRow_(0),
      filterType_(CategoryFilterType::None),
      sortField_(CategorySortField::None),
      sortAsc_(true),
      keyword_(""),
      topActionRow_(3),
      topActionIndex_(0),
      onTopAction_(true) {
    reloadData();
}

string CategoryListScreen::buildFilterLabel() const {
    if (filterType_ == CategoryFilterType::Name)
        return string("Ten chua \"") + keyword_ + "\"";
    return string();
}

string CategoryListScreen::buildSortLabel() const {
    if (sortField_ == CategorySortField::Name)
        return string("Sap xep ten (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
    return string();
}

void CategoryListScreen::updateViewNoteFromState() {
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

void CategoryListScreen::drawOptionRow(short startRow, const vector<string> &options, int index, bool selected) const {
    gotoRC(startRow + index, 0);
    clearLine();
    cout << bullet(selected) << options[index];
}

void CategoryListScreen::drawMenuOptions(short startRow, const vector<string> &options, int sel) const {
    for (size_t i = 0; i < options.size(); ++i) {
        drawOptionRow(startRow, options, (int)i, (int)i == sel);
    }
}

bool CategoryListScreen::promptInlineString(short row, const string &prefix, string &value) {
    return ::promptInlineText(row, prefix, value) == 1;
}

void CategoryListScreen::reloadData() {
    if (filterType_ == CategoryFilterType::Name) {
        items_ = catBLL_.searchByName(keyword_);
    } else {
        items_ = catBLL_.getAll();
    }
    applySort();
    calculatePagination();
}

void CategoryListScreen::applySort() {
    if (sortField_ == CategorySortField::None)
        return;

    if (filterType_ == CategoryFilterType::None) {
        items_ = catBLL_.getAllSortedByName(sortAsc_);
        return;
    }

    sort(items_.begin(), items_.end(), [&](const Category &a, const Category &b) {
        return sortAsc_ ? a.name < b.name : a.name > b.name;
    });
}

void CategoryListScreen::resetView() {
    filterType_ = CategoryFilterType::None;
    sortField_ = CategorySortField::None;
    sortAsc_ = true;
    keyword_.clear();
    viewNote_.clear();
}

bool CategoryListScreen::promptStringInput(const string &title, const string &label, string &out) {
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
        if (ch == 13) {
            out = buffer;
            hideCursor();
            return true;
        } else if (ch == 27) {
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

bool CategoryListScreen::promptSortOrder(bool &asc) {
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

vector<CategoryListScreen::CategoryAction> CategoryListScreen::topActions() const {
    vector<CategoryAction> acts;
    if (!selectMode_)
        acts.push_back(CategoryAction::Add);
    acts.push_back(CategoryAction::Search);
    acts.push_back(CategoryAction::Sort);
    return acts;
}

void CategoryListScreen::drawTopActions() {
    vector<CategoryAction> actions = topActions();
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
        case CategoryAction::Add:
            cout << "[Them]";
            break;
        case CategoryAction::Search:
            cout << "[Tim kiem]";
            break;
        case CategoryAction::Sort:
            cout << "[Sap xep]";
            break;
        }
    }
    cout.flush();
}

void CategoryListScreen::handleTopAction(CategoryAction action) {
    if (action == CategoryAction::Add) {
        Category init;
        init.id = 0;
        init.name = "";
        app->push(new CategoryEditScreen(app, true, init));
        return;
    }
    if (action == CategoryAction::Search) {
        openSearchMenu();
        return;
    }
    openSortMenu();
}

void CategoryListScreen::openSearchMenu() {
    inAction_ = false;
    msg_.clear();

    vector<string> options = {"Hien tat ca", "Tim theo ten"};
    int sel = (filterType_ == CategoryFilterType::Name) ? 1 : 0;

    cls();
    printTitleBox("TIM KIEM DANH MUC");
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
                int result = promptInlineText(menuStartRow + sel, options[sel], kw);
                if (result == 1) {
                    filterType_ = CategoryFilterType::Name;
                    keyword_ = kw;
                    updateViewNoteFromState();
                    changed = true;
                    break;
                } else
                    redrawItem(sel, true);
            }
        }
    }
    if (changed)
        reloadData();
    onTopAction_ = true;
    draw();
}

void CategoryListScreen::openSortMenu() {
    inAction_ = false;
    msg_.clear();

    vector<pair<string, CategorySortField>> options = {
        //{"Bo sap xep", CategorySortField::None},
        {"Theo ten", CategorySortField::Name}};

    int sel = 0;
    bool asc = sortAsc_;
    for (size_t i = 0; i < options.size(); ++i) {
        if (sortField_ == options[i].second) {
            sel = (int)i;
            break;
        }
    }

    cls();
    printTitleBox("SAP XEP DANH MUC");
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
    printHint("[Up/Down] Chon truong  |  [Enter] Thuc hien  |  [Backspace]/[Esc] Huy");
    cout.flush();

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
            CategorySortField chosen = options[sel].second;
            if (chosen == CategorySortField::None) {
                sortField_ = CategorySortField::None;
                sortAsc_ = true;
                updateViewNoteFromState();
                changed = true;
                break;
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
        }
    }

exit_sort_menu:
    if (changed)
        reloadData();
    onTopAction_ = true;
    draw();
}

void CategoryListScreen::drawTableHeader() {
    const int tableWidth = 42;
    printDivider('=', tableWidth);
    setColor(COLOR_ACCENT);
    stringstream hs;
    hs << "  "
       << left << setw(6) << " ID"
       << left << setw(30) << "Ten danh muc";
    cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
    resetColor();
    printDivider('-', tableWidth);
}

void CategoryListScreen::drawRowItem(int index, const Category &item, bool selected) {
    const int tableWidth = 42;
    int relativeRow = firstRow_ + (index % ITEMS_PER_PAGE);
    gotoRC(relativeRow, 0);
    clearLine();

    bool showArrow = (selected && !inAction_ && !onTopAction_);

    stringstream ss;
    ss << bullet(showArrow)
       << left << setw(4) << item.id
       << left << setw(30) << item.name;
    cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |";

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

void CategoryListScreen::draw() {
    reloadData();
    cls();

    string title = selectMode_ ? "CHON DANH MUC" : "QUAN LY DANH MUC";
    string subtitle = selectMode_ ? "Len/Xuong de chon, Enter de ap dung" : "Enter de Them hoac mo menu Sua/Xoa";
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
        printHint("[Up/Down] chon danh muc  |  [Left/Right] chon Tim/Sap xep  |  [Enter] thuc hien  |  [Backspace] quay lai");
    } else {
        if (!inAction_) {
            printHint("[Up/Down] chon  |  [Left/Right] chon Them/Tim/Sap xep  |  [Enter] thuc hien  |  [Backspace] quay lai");
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

void CategoryListScreen::onKey(Key k) {
    if (showingMsg_) {
        showingMsg_ = false;
        msg_.clear();
        gotoRC(msgRow_, 0);
        clearLine();
        cout.flush();
    }

    if (onTopAction_) {
        vector<CategoryAction> actions = topActions();
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
            msg_.clear();
            if (selectedIndex_ == 0 && currentPage_ == 1) {
                onTopAction_ = true;
                redrawRow(selectedIndex_, false);
                drawTopActions();
                return;
            }
        } else if (k == KEY_ENTER) {
            msg_.clear();
            if (selectMode_) {
                if (foodScreen_ != NULL && selectedIndex_ >= 0) {
                    foodScreen_->setCategory(items_[selectedIndex_].id);
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
                Category c = items_[selectedIndex_];
                if (actionIndex_ == 0) {
                    app->push(new CategoryEditScreen(app, false, c));
                    inAction_ = false;
                } else {
                    Result r = catBLL_.removeChecked(c.id);
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

// ======================= CategoryEditScreen =======================
CategoryEditScreen::CategoryEditScreen(App *a, bool isNew, const Category &initial)
    : Screen(a),
      catBLL_(a->category()),
      isNew_(isNew),
      cat_(initial),
      focus_(0),
      msg_("") {}

void CategoryEditScreen::drawForm() const {
    cls();
    gotoRC(0, 0);

    cout << (isNew_ ? "=== THEM DANH MUC ===\n\n" : "=== SUA DANH MUC ===\n\n");

    cout << (focus_ == 0 ? "> " : "  ");
    cout << "Ten danh muc: " << cat_.name << "\n\n";

    cout << (focus_ == 1 ? "> " : "  ");
    cout << "[Xac nhan]\n\n";

    if (!msg_.empty()) {
        cout << msg_ << "\n\n";
    }

    cout << "[Up/Down] chon field  |  [Enter] edit/xac nhan  |  [Backspace] quay lai  |  [Esc] thoat\n";
    cout.flush();
}

void CategoryEditScreen::draw() {
    drawForm();
}

void CategoryEditScreen::redrawFocusLines() {
    gotoRC(2, 0);
    clearLine();
    cout << (focus_ == 0 ? "> " : "  ")
         << "Ten danh muc: " << cat_.name;

    gotoRC(4, 0);
    clearLine();
    cout << (focus_ == 1 ? "> " : "  ")
         << "[Xac nhan]";

    cout.flush();
}

void CategoryEditScreen::showMsg() {
    gotoRC(8, 0);
    clearLine();
    if (!msg_.empty())
        cout << msg_;
    cout.flush();
}

void CategoryEditScreen::editName(Key k) {
    string buffer = cat_.name;
    showCursor();

    while (true) {
        gotoRC(2, 0);
        clearLine();
        cout << (focus_ == 0 ? "> " : "  ")
             << "Ten danh muc: " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            cat_.name = buffer;
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

void CategoryEditScreen::save() {
    Result r;
    if (isNew_) {
        r = catBLL_.addChecked(cat_);
    } else {
        r = catBLL_.updateChecked(cat_);
    }

    msg_ = r.message;
    showMsg();
}

void CategoryEditScreen::onKey(Key k) {
    bool focusChanged = false;

    if (k == KEY_UP && focus_ > 0) {
        focus_--;
        focusChanged = true;
    } else if (k == KEY_DOWN && focus_ < 1) {
        focus_++;
        focusChanged = true;
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    } else if (k == KEY_ENTER) {
        if (focus_ == 0) {
            editName(k);
        } else if (focus_ == 1) {
            save();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}
