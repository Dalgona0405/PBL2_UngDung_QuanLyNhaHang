// TableUI.cpp
#include "TableUI.h"
#include "BillUI.h"
#include "ConsoleUtils.h"
#include "SearchSortUtils.h"
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

static string uiStatusLabel(int status) {
    switch (status) {
    case 0:
        return string("Trong");
    case 1:
        return string("Dang su dung");
    default:
        return string("Khong xac dinh");
    }
}

void TableListScreen::applySort() {
    if (sortField_ == TableSortField::None)
        return;

    if (filterType_ == TableFilterType::None) {
        if (sortField_ == TableSortField::Number) {
            items_ = tableBLL_.getAllSortedByNumber(sortAsc_);
        } else if (sortField_ == TableSortField::Capacity) {
            items_ = tableBLL_.getAllSortedByCapacity(sortAsc_);
        } else if (sortField_ == TableSortField::Status) {
            items_ = tableBLL_.getAllSortedByStatus(sortAsc_);
        }
        return;
    }

    sort(items_.begin(), items_.end(), [&](const Table &a, const Table &b) {
        if (sortField_ == TableSortField::Number) {
            return sortAsc_ ? a.number < b.number : a.number > b.number;
        }
        if (sortField_ == TableSortField::Capacity) {
            return sortAsc_ ? a.capacity < b.capacity : a.capacity > b.capacity;
        }
        if (sortField_ == TableSortField::Status) {
            return sortAsc_ ? a.status_id < b.status_id : a.status_id > b.status_id;
        }
        return false;
    });
}
void TableListScreen::resetView() {
    filterType_ = TableFilterType::None;
    sortField_ = TableSortField::None;
    sortAsc_ = true;
    filterNumber_ = 0;
    filterCapacity_ = 0;
    filterStatus_ = -1;
    viewNote_.clear();
}

bool TableListScreen::promptIntInput(const string &title, const string &label, int &out) {
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

bool TableListScreen::promptSortOrder(bool &asc) {
    cls();
    printTitleBox("Chon thu tu sap xep");
    printDivider('=');
    vector<string> options = {"Tang dan", "Giam dan"};
    int sel = asc ? 0 : 1;
    short startRow = contentRow();

    auto redraw = [&](int index, bool selected) {
        gotoRC(startRow + index, 0);
        clearLine();
        cout << bullet(selected) << options[index];
    };

    for (size_t i = 0; i < options.size(); ++i) {
        cout << bullet((int)i == sel) << options[i] << "\n";
    }
    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] xac nhan  |  [Backspace]/[Esc] huy");
    cout.flush();

    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC) {
            return false;
        } else if (k == KEY_UP && sel > 0) {
            redraw(sel, false);
            sel--;
            redraw(sel, true);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            redraw(sel, false);
            sel++;
            redraw(sel, true);
        } else if (k == KEY_ENTER) {
            asc = (sel == 0);
            return true;
        }
    }
}

bool TableListScreen::promptStatus(int &status) {
    cls();
    printTitleBox("Chon trang thai ban");
    printDivider('=');
    vector<string> options = {"Trong", "Dang su dung"};
    int sel = (status == 1) ? 1 : 0;
    short startRow = contentRow();

    auto redraw = [&](int index, bool selected) {
        gotoRC(startRow + index, 0);
        clearLine();
        cout << bullet(selected) << options[index];
    };

    for (size_t i = 0; i < options.size(); ++i) {
        cout << bullet((int)i == sel) << options[i] << "\n";
    }
    cout << "\n";
    printHint("[Up/Down] chon  |  [Enter] xac nhan  |  [Backspace]/[Esc] huy");
    cout.flush();

    while (true) {
        Key k = readKey();
        if (k == KEY_BACKSPACE || k == KEY_ESC) {
            return false;
        } else if (k == KEY_UP && sel > 0) {
            redraw(sel, false);
            sel--;
            redraw(sel, true);
        } else if (k == KEY_DOWN && sel + 1 < (int)options.size()) {
            redraw(sel, false);
            sel++;
            redraw(sel, true);
        } else if (k == KEY_ENTER) {
            status = sel;
            return true;
        }
    }
}

vector<TableListScreen::TableAction> TableListScreen::topActions() const {
    vector<TableAction> acts;
    if (!selectMode_)
        acts.push_back(TableAction::Add);
    acts.push_back(TableAction::Search);
    acts.push_back(TableAction::Sort);
    return acts;
}

void TableListScreen::drawTopActions() {
    auto actions = topActions();
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
        case TableAction::Add:
            cout << "[Them]";
            break;
        case TableAction::Search:
            cout << "[Tim kiem]";
            break;
        case TableAction::Sort:
            cout << "[Sap xep]";
            break;
        }
    }
    cout.flush();
}

void TableListScreen::handleTopAction(TableAction action) {
    if (action == TableAction::Add) {
        Table init;
        init.id = 0;
        init.number = 0;
        init.capacity = 0;
        init.status_id = 0;
        app->push(new TableEditScreen(app, true, init));
        return;
    }

    if (action == TableAction::Search) {
        openSearchMenu();
        return;
    }

    openSortMenu();
}

void TableListScreen::openSearchMenu() {
    inAction_ = false;
    msg_.clear();

    auto buildFilterLabel = [&]() -> string {
        if (filterType_ == TableFilterType::Number)
            return string("So ban = ") + to_string(filterNumber_);
        if (filterType_ == TableFilterType::Capacity)
            return string("Suc chua = ") + to_string(filterCapacity_);
        if (filterType_ == TableFilterType::Status)
            return string("Trang thai: ") + statusLabel(filterStatus_);
        return string();
    };

    auto buildSortLabel = [&]() -> string {
        if (sortField_ == TableSortField::Number)
            return string("Sap xep so ban (") + (sortAsc_ ? "tang" : "giam") + ")";
        if (sortField_ == TableSortField::Capacity)
            return string("Sap xep suc chua (") + (sortAsc_ ? "tang" : "giam") + ")";
        if (sortField_ == TableSortField::Status)
            return string("Sap xep trang thai (") + (sortAsc_ ? "trong->co khach" : "co khach->trong") + ")";
        return string();
    };

    auto updateViewNote = [&]() {
        string note = buildFilterLabel();
        string sortLabel = buildSortLabel();
        if (!sortLabel.empty()) {
            if (!note.empty())
                note += " | " + sortLabel;
            else
                note = sortLabel;
        }
        viewNote_ = note;
    };

    vector<string> options = {"Hien tat ca", "Tim theo so ban", "Tim theo suc chua", "Tim theo trang thai"};
    int sel = 0;
    if (filterType_ == TableFilterType::Number)
        sel = 1;
    else if (filterType_ == TableFilterType::Capacity)
        sel = 2;
    else if (filterType_ == TableFilterType::Status)
        sel = 3;

    cls();
    printTitleBox("TIM KIEM BAN");
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
            gotoRC(menuStartRow + (int)i, 0);
            clearLine();
            cout << bullet((int)i == sel) << options[i];
        }
        cout.flush();
    };

    auto promptIntInline = [&](int row, const string &label, int &value) -> int {
        string caption = options[sel];
        return ::promptInlineInt(row, caption, value);
    };

    auto promptStatusInline = [&](int row, int &status) -> bool {
        int local = (status < 0) ? 0 : status;
        while (true) {
            gotoRC(row, 0);
            clearLine();
            cout << bullet(true) << options[sel] << " : "
                 << (local == 0 ? "[*Trong*] " : "[Trong] ")
                 << (local == 1 ? "[*Dang su dung*]" : "[Dang su dung]");
            cout.flush();
            Key k2 = readKey();
            if (k2 == KEY_LEFT) {
                local = 0;
            } else if (k2 == KEY_RIGHT) {
                local = 1;
            } else if (k2 == KEY_ENTER) {
                status = local;
                return true;
            } else if (k2 == KEY_BACKSPACE || k2 == KEY_ESC) {
                return false;
            }
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
                updateViewNote();
                changed = true;
                break;
            } else if (sel == 1) {
                int num = filterNumber_;
                int result = promptIntInline(menuStartRow + sel, "So", num);
                if (result == 1) {
                    filterType_ = TableFilterType::Number;
                    filterNumber_ = num;
                    updateViewNote();
                    changed = true;
                    break;
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 2) {
                int cap = filterCapacity_;
                int result = promptIntInline(menuStartRow + sel, "Suc chua", cap);
                if (result == 1) {
                    filterType_ = TableFilterType::Capacity;
                    filterCapacity_ = cap;
                    updateViewNote();
                    changed = true;
                    break;
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 3) {
                int st = (filterStatus_ < 0) ? 0 : filterStatus_;
                if (promptStatusInline(menuStartRow + sel, st)) {
                    filterType_ = TableFilterType::Status;
                    filterStatus_ = st;
                    updateViewNote();
                    changed = true;
                    break;
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

void TableListScreen::openSortMenu() {
    inAction_ = false;
    msg_.clear();

    auto buildFilterLabel = [&]() -> string {
        if (filterType_ == TableFilterType::Number)
            return string("So ban = ") + to_string(filterNumber_);
        if (filterType_ == TableFilterType::Capacity)
            return string("Suc chua = ") + to_string(filterCapacity_);
        if (filterType_ == TableFilterType::Status)
            return string("Trang thai: ") + statusLabel(filterStatus_);
        return string();
    };

    auto buildSortLabel = [&]() -> string {
        if (sortField_ == TableSortField::Number)
            return string("Sap xep so ban (") + (sortAsc_ ? "tang" : "giam") + ")";
        if (sortField_ == TableSortField::Capacity)
            return string("Sap xep suc chua (") + (sortAsc_ ? "tang" : "giam") + ")";
        if (sortField_ == TableSortField::Status)
            return string("Sap xep trang thai (") + (sortAsc_ ? "trong->co khach" : "co khach->trong") + ")";
        return string();
    };

    auto updateViewNote = [&]() {
        string note = buildFilterLabel();
        string sortLabel = buildSortLabel();
        if (!sortLabel.empty()) {
            if (!note.empty())
                note += " | " + sortLabel;
            else
                note = sortLabel;
        }
        viewNote_ = note;
    };

    vector<pair<string, TableSortField>> options = {
        /*{"Bo sap xep", TableSortField::None},*/
        {"So ban", TableSortField::Number},
        {"Suc chua", TableSortField::Capacity},
        {"Trang thai", TableSortField::Status}};

    int sel = 0;
    bool asc = sortAsc_;
    for (size_t i = 0; i < options.size(); ++i) {
        if (sortField_ == options[i].second) {
            sel = (int)i;
            break;
        }
    }

    cls();
    printTitleBox("SAP XEP BAN");
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
            TableSortField chosen = options[sel].second;
            if (chosen == TableSortField::None) {
                sortField_ = TableSortField::None;
                sortAsc_ = true;
                updateViewNote();
                changed = true;
            } else {
                // Inline order picker on the same row
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
                        updateViewNote();
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
    if (changed)
        reloadData();
    onTopAction_ = true;
    draw();
}

void TableListScreen::drawTableHeader() {
    const int tableWidth = 47;
    printDivider('=', tableWidth);
    setColor(COLOR_ACCENT);
    stringstream hs;

    hs << "  "
       << left << setw(6) << " ID"
       << left << setw(10) << "So ban"
       << left << setw(10) << "Suc chua"
       << left << setw(15) << "Trang thai";
    cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
    resetColor();
    printDivider('-', tableWidth);
}

// ======================= TableListScreen =======================
TableListScreen::TableListScreen(App *a, bool selectMode, BillListScreen *billScreen)
    : PagedListScreen<Table>(a),
      tableBLL_(a->table()),
      selectMode_(selectMode),
      billScreen_(billScreen),
      inAction_(false),
      actionIndex_(0),
      msg_(""),
      showingMsg_(false),
      viewNote_(""),
      msgRow_(0),
      filterType_(TableFilterType::None),
      sortField_(TableSortField::None),
      sortAsc_(true),
      filterNumber_(0),
      filterCapacity_(0),
      filterStatus_(-1),
      topActionRow_(3),
      topActionIndex_(0),
      onTopAction_(true) {
    reloadData();
}

string TableListScreen::statusLabel(int status) const {
    return uiStatusLabel(status);
}

void TableListScreen::reloadData() {
    if (filterType_ == TableFilterType::Number) {
        items_ = tableBLL_.searchByNumber(filterNumber_);
    } else if (filterType_ == TableFilterType::Capacity) {
        items_ = tableBLL_.searchByCapacity(filterCapacity_);
    } else if (filterType_ == TableFilterType::Status) {
        items_ = tableBLL_.searchByStatus(filterStatus_);
    } else {
        items_ = tableBLL_.getAll();
    }
    applySort();
    calculatePagination();
}

void TableListScreen::drawRowItem(int index, const Table &item, bool selected) {
    const int tableWidth = 47;
    int relativeRow = firstRow_ + (index % ITEMS_PER_PAGE);
    gotoRC(relativeRow, 0);
    clearLine();

    bool showArrow = (selected && !inAction_ && !onTopAction_);

    stringstream ss;
    ss << bullet(showArrow)
       << left << setw(6) << item.id
       << left << setw(10) << item.number
       << left << setw(10) << item.capacity
       << left << setw(15) << statusLabel(item.status_id);
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

void TableListScreen::draw() {
    reloadData();
    cls();

    string title = selectMode_ ? "CHON BAN" : "QUAN LY BAN";
    string subtitle = selectMode_ ? "Chon ban, Enter de phuc vu" : "Enter de Them hoac mo menu Sua/Xoa";
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
        printHint("[Up/Down] chon ban  |  [Left/Right] chon Tim/Sap xep  |  [Enter] thuc hien");
    } else {
        if (!inAction_) {
            printHint("[Up/Down] chon ban  |  [Left/Right] chon Them/Tim/Sap xep  |  [Enter] thuc hien  |  [Backspace] quay lai");
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
void TableListScreen::onKey(Key k) {
    if (showingMsg_) {
        showingMsg_ = false;
        msg_.clear();
        gotoRC(msgRow_, 0);
        clearLine();
        cout.flush();
    }

    if (onTopAction_) {
        vector<TableAction> actions = topActions();
        int oldTop = topActionIndex_;
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
            if (!items_.empty()) {
                onTopAction_ = false;
                drawTopActions();
                redrawRow(selectedIndex_, true);
                cout.flush();
            }
            return;
        } else if (k == KEY_ENTER) {
            handleTopAction(actions[topActionIndex_]);
            return;
        } else if (k == KEY_BACKSPACE) {
            // Khong cho phep backspace khi dang chon ban
            if (!selectMode_) {
                app->pop();
                return;
            }
        }
        if (oldTop != topActionIndex_)
            drawTopActions();
        return;
    } else if (!inAction_) {
        int nav = handleNavigationKey(k);
        if (nav == 2) {
            msg_.clear();
            draw();
            return;
        }
        if (nav == 1) {
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
                if (billScreen_ != NULL && selectedIndex_ >= 0) {
                    billScreen_->setTable(items_[selectedIndex_]);
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
            // Khong cho phep backspace khi dang chon ban
            if (!selectMode_) {
                app->pop();
                return;
            }
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
                Table t = items_[selectedIndex_];
                if (actionIndex_ == 0) {
                    app->push(new TableEditScreen(app, false, t));
                    inAction_ = false;
                } else {
                    Result r = tableBLL_.removeChecked(t.id);
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
        if (oldAction != actionIndex_) {
            redrawRow(selectedIndex_, true);
            cout.flush();
        }
    }
}

// ======================= TableEditScreen =======================
TableEditScreen::TableEditScreen(App *a, bool isNew, const Table &initial)
    : Screen(a), tableBLL_(a->table()), isNew_(isNew), table_(initial), focus_(0), msg_("") {
    if (isNew_) {
        table_.status_id = 0;
    }
}

TableEditScreen::~TableEditScreen() {}

void TableEditScreen::draw() {
    drawForm();
}

void TableEditScreen::drawForm() const {
    cls();
    gotoRC(0, 0);

    cout << (isNew_ ? "=== THEM BAN ===\n\n" : "=== SUA BAN ===\n\n");

    cout << (focus_ == 0 ? "> " : "  ") << "So ban    : " << table_.number << "\n";
    cout << (focus_ == 1 ? "> " : "  ") << "Suc chua  : " << table_.capacity << "\n";
    cout << "Trang thai: " << uiStatusLabel(table_.status_id) << "\n\n";
    cout << (focus_ == 2 ? "> " : "  ") << "[Xac nhan]\n\n";

    if (!msg_.empty()) {
        cout << msg_ << "\n\n";
    }

    printHint("[Up/Down] chon field  |  [Enter] edit  |  [Backspace] quay lai  |  [Esc] thoat");
    cout.flush();
}

void TableEditScreen::redrawFocusLines() {
    gotoRC(2, 0);
    clearLine();
    cout << (focus_ == 0 ? "> " : "  ") << "So ban    : " << table_.number;

    gotoRC(3, 0);
    clearLine();
    cout << (focus_ == 1 ? "> " : "  ") << "Suc chua  : " << table_.capacity;

    gotoRC(6, 0);
    clearLine();
    cout << (focus_ == 2 ? "> " : "  ") << "[Xac nhan]";

    gotoRC(4, 0);
    clearLine();
    cout << "Trang thai: " << uiStatusLabel(table_.status_id);

    cout.flush();
}

void TableEditScreen::showMsg() {
    gotoRC(10, 0);
    clearLine();
    if (!msg_.empty())
        cout << msg_;
    cout.flush();
}

void TableEditScreen::editNumber(Key k) {
    string buffer;
    if (table_.number > 0) {
        char tmp[32];
        sprintf(tmp, "%d", table_.number);
        buffer = tmp;
    }
    showCursor();

    while (true) {
        gotoRC(2, 0);
        clearLine();
        cout << (focus_ == 0 ? "> " : "  ") << "So ban    : " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            if (!buffer.empty()) {
                int v = atoi(buffer.c_str());
                if (v < 0)
                    v = 0;
                table_.number = v;
            } else {
                table_.number = 0;
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

void TableEditScreen::editCapacity(Key k) {
    string buffer;
    if (table_.capacity > 0) {
        char tmp[32];
        sprintf(tmp, "%d", table_.capacity);
        buffer = tmp;
    }
    showCursor();

    while (true) {
        gotoRC(3, 0);
        clearLine();
        cout << (focus_ == 1 ? "> " : "  ") << "Suc chua  : " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            if (!buffer.empty()) {
                int v = atoi(buffer.c_str());
                if (v < 0)
                    v = 0;
                table_.capacity = v;
            } else {
                table_.capacity = 0;
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

void TableEditScreen::save() {
    Result r;
    if (isNew_) {
        table_.status_id = 0;
        r = tableBLL_.addChecked(table_);
    } else {
        r = tableBLL_.updateChecked(table_);
    }

    msg_ = r.message;
    showMsg();
}

void TableEditScreen::onKey(Key k) {
    bool focusChanged = false;

    if (k == KEY_UP && focus_ > 0) {
        focus_--;
        focusChanged = true;
    } else if (k == KEY_DOWN && focus_ < 2) {
        focus_++;
        focusChanged = true;
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    } else if (k == KEY_ENTER) {
        if (focus_ == 0) {
            editNumber(k);
        } else if (focus_ == 1) {
            editCapacity(k);
        } else if (focus_ == 2) {
            save();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}