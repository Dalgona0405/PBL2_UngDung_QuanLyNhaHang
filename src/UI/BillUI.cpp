#include "BillUI.h"
#include "ConsoleUtils.h"
#include "FoodUI.h"
#include "SearchSortUtils.h"
#include "TableUI.h"
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// ======================= BillListScreen =======================
BillListScreen::BillListScreen(App *a)
    : Screen(a),
      billBLL_(a->bill()),
      foodBLL_(a->food()),
      tableBLL_(a->table()),
      total_(0),
      hasTable_(false),
      autoChooseTable_(true),
      selectedIndex_(-1),
      inAction_(false),
      actionIndex_(0),
      msg_(""),
      showingMsg_(false),
      msgRow_(0),
      firstRow_(7),
      addRow_(3) {}

void BillListScreen::chooseTable() {
    app->push(new TableListScreen(app, true, this));
}

void BillListScreen::setTable(const Table &t) {
    table_ = t;
    hasTable_ = true;
    selectedIndex_ = -1;
    inAction_ = false;
}

void BillListScreen::reload() {
    items_.clear();
    bill_ = Bill();
    total_ = 0;

    if (!hasTable_)
        return;

    billBLL_.getCurrentItemsOfTable(table_.id, items_, &bill_, &total_);

    int n = (int)items_.size();
    if (n == 0) {
        selectedIndex_ = -1;
    } else {
        if (selectedIndex_ > n)
            selectedIndex_ = n;
        if (selectedIndex_ < -1)
            selectedIndex_ = -1;
    }
}

void BillListScreen::drawRow(int index, bool selected) {
    if (index < 0 || index >= (int)items_.size())
        return;

    const BillItem &bi = items_[index];
    auto shortNote = [](const string &s) {
        if (s.size() <= 20)
            return s;
        return s.substr(0, 17) + "...";
    };

    Food f;
    string foodName = "(?)";
    double unitPrice = 0;

    if (foodBLL_.getById(bi.food_id, f)) {
        foodName = f.name;
        unitPrice = f.price;
    } else if (bi.quantity > 0) {
        unitPrice = bi.sub_total / bi.quantity;
    }

    int row = firstRow_ + index;
    gotoRC(row, 0);
    clearLine();

    bool showArrow = (selected && !inAction_);
    const int tableWidth = 95;
    stringstream ss;
    ss << bullet(showArrow)
       << left << setw(4) << bi.id
       << left << setw(30) << foodName
       << left << setw(10) << bi.quantity
       << left << setw(10) << (long long)unitPrice
       << left << setw(12) << (long long)bi.sub_total
       << shortNote(bi.description);
    cout << "| " << left << setw(tableWidth - 4) << ss.str() << " |";

    if (selected && inAction_) {
        cout << "   ";
        setColor(COLOR_ACCENT);
        cout << ((actionIndex_ == 0) ? "[*Sua*]" : "[Sua]");
        resetColor();
        cout << " ";
        setColor(COLOR_ALERT);
        cout << ((actionIndex_ == 1) ? "[*Xoa*]" : "[Xoa]");
        resetColor();
    }

    cout.flush();
}

void BillListScreen::draw() {
    if (!hasTable_) {
        if (autoChooseTable_) {
            autoChooseTable_ = false;
            chooseTable();

            Screen *top = app->top();
            if (top != NULL && top != this) {
                top->draw();
            }
            return;
        } else {
            app->pop();
            return;
        }
    }

    reload();
    cls();
    inAction_ = false;

    string title = "HOA DON BAN #" + to_string(table_.number);
    string subtitle = "Up/Down de chon mon | Enter de sua/xoa/thanh toan";
    printTitleBox(title, subtitle);
    printDivider('=');

    addRow_ = contentRow();
    firstRow_ = addRow_ + 4; // fallback; overwritten when list exists

    bool addSel = (selectedIndex_ == -1 && !inAction_);
    gotoRC(addRow_, 0);
    clearLine();
    cout << bullet(addSel) << "[Them mon an]\n";

    int n = (int)items_.size();

    if (n > 0) {
        cout << "\n";
        const int tableWidth = 95;
        printDivider('=', tableWidth);
        setColor(COLOR_ACCENT);
        stringstream hs;
        hs << left << setw(6) << "  ID"
           << left << setw(30) << "Ten mon"
           << left << setw(10) << "So luong"
           << left << setw(10) << "Don gia"
           << left << setw(12) << "Thanh tien"
           << "Ghi chu";
        cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
        resetColor();
        printDivider('-', tableWidth);

        firstRow_ = contentRow();

        for (int i = 0; i < n; ++i) {
            bool sel = (selectedIndex_ == i && !inAction_);
            drawRow(i, sel);
            cout << "\n";
        }
        printDivider('=', tableWidth);

        int rowAfterList = firstRow_ + n + 1;

        gotoRC(rowAfterList, 0);
        clearLine();
        setColor(COLOR_ACCENT);
        cout << "Tong gia tien: " << (long long)total_ << "\n";
        resetColor();

        bool paySel = (selectedIndex_ == n && !inAction_);
        gotoRC(rowAfterList + 1, 0);
        clearLine();
        cout << bullet(paySel) << "[Thanh toan]";
        firstRow_ = firstRow_; // keep for footer calc
    } else {
        // no items: reset selection to add item
        selectedIndex_ = -1;
        inAction_ = false;
        gotoRC(addRow_ + 2, 0);
        clearLine();
        setColor(COLOR_MUTED);
        cout << "(Chua co mon nao trong hoa don)\n";
        resetColor();
        // ensure pay button line is cleared
        gotoRC(addRow_ + 3, 0);
        clearLine();
        firstRow_ = addRow_ + 2;
    }

    int footerRow = firstRow_ + (n > 0 ? n + 4 : 4);
    gotoRC(footerRow, 0);
    clearLine();
    if (!inAction_) {
        printHint("[Up/Down] chon  |  [Enter] Them/Sua/Xoa/Thanh toan  |  [Backspace] quay lai");
    } else {
        printHint("[Left/Right] chon Sua/Xoa  |  [Enter] thuc hien  |  [Backspace] huy menu");
    }

    msgRow_ = footerRow + 2;
    if (!msg_.empty()) {
        gotoRC(msgRow_, 0);
        clearLine();
        setColor(COLOR_ALERT);
        cout << msg_;
        resetColor();
    }

    cout.flush();
}

void BillListScreen::onKey(Key k) {
    // Neu dang hien thi message, bat ky phim nao cung clear message
    if (showingMsg_) {
        showingMsg_ = false;
        msg_.clear();
        // Chi xoa dong message thay vi ve lai toan bo
        gotoRC(msgRow_, 0);
        clearLine();
        cout.flush();
    }

    if (!hasTable_) {
        return;
    }
    msg_.clear();

    int oldSel = selectedIndex_;
    bool oldInAction = inAction_;
    int oldAction = actionIndex_;

    int n = (int)items_.size();
    int maxIndex = (n > 0) ? n : -1;

    if (!inAction_) {
        if (k == KEY_UP) {
            if (selectedIndex_ > -1)
                selectedIndex_--;
        } else if (k == KEY_DOWN) {
            if (selectedIndex_ < maxIndex)
                selectedIndex_++;
        } else if (k == KEY_ENTER) {
            if (selectedIndex_ == -1) {
                BillItem init;
                init.id = 0;
                init.bill_id = bill_.id;
                init.food_id = 0;
                init.quantity = 1;
                init.description = "";
                init.sub_total = 0;
                app->push(new BillItemEditScreen(app, table_, init, true));
                return;
            } else if (selectedIndex_ >= 0 && selectedIndex_ < n) {
                inAction_ = true;
                actionIndex_ = 0;
            } else if (selectedIndex_ == n && n > 0) {
                Result r = billBLL_.checkoutTable(table_.id);
                msg_ = r.message;

                reload();
                selectedIndex_ = -1;
                inAction_ = false;

                draw();
                return;
            }
        } else if (k == KEY_BACKSPACE) {
            app->pop();
            return;
        }
    } else {
        if (k == KEY_LEFT) {
            if (actionIndex_ > 0)
                actionIndex_--;
        } else if (k == KEY_RIGHT) {
            if (actionIndex_ < 1)
                actionIndex_++;
        } else if (k == KEY_BACKSPACE) {
            inAction_ = false;
            msg_.clear();
        } else if (k == KEY_ENTER) {
            if (selectedIndex_ >= 0 && selectedIndex_ < n) {
                BillItem bi = items_[selectedIndex_];
                if (actionIndex_ == 0) {
                    app->push(new BillItemEditScreen(app, table_, bi, false));
                    return;
                } else {
                    Result r = billBLL_.deleteBillItem(bi.id);
                    msg_ = r.message;
                    if (r.ok) {
                        showingMsg_ = true;
                        reload();
                        inAction_ = false;
                        if (selectedIndex_ >= (int)items_.size())
                            selectedIndex_ = (int)items_.size() - 1;
                        draw();
                        return;
                    } else {
                        showingMsg_ = true;
                        inAction_ = false;
                    }
                }
            } else {
                inAction_ = false;
            }
        }
    }

    if (!inAction_ && selectedIndex_ != oldSel) {
        if (oldSel >= 0 && oldSel < n)
            drawRow(oldSel, false);
        if (selectedIndex_ >= 0 && selectedIndex_ < n)
            drawRow(selectedIndex_, true);

        if (addRow_ != -1 && (oldSel == -1 || selectedIndex_ == -1)) {
            gotoRC(addRow_, 0);
            clearLine();
            bool addSel = (selectedIndex_ == -1);
            cout << bullet(addSel) << "[Them mon an]";
        }

        if (n > 0 && (oldSel == n || selectedIndex_ == n)) {
            int rowAfterList = firstRow_ + n + 1;
            gotoRC(rowAfterList + 1, 0);
            clearLine();
            bool paySel = (selectedIndex_ == n);
            cout << bullet(paySel) << "[Thanh toan]";
        }

        cout.flush();
    }

    if (inAction_ || oldInAction || actionIndex_ != oldAction) {
        int rowIdx = (selectedIndex_ >= 0) ? selectedIndex_ : oldSel;
        if (rowIdx >= 0 && rowIdx < n) {
            drawRow(rowIdx, true);
        }
    }
}

// ======================= BillItemEditScreen =======================
BillItemEditScreen::BillItemEditScreen(App *a, const Table &table, const BillItem &initial, bool isNew)
    : Screen(a),
      billBLL_(a->bill()),
      foodBLL_(a->food()),
      table_(table),
      item_(initial),
      isNew_(isNew),
      hasFood_(false),
      quantity_(1),
      note_(""),
      focus_(0),
      msg_("") {
    if (!isNew_) {
        quantity_ = initial.quantity;
        note_ = initial.description;

        Food f;
        if (foodBLL_.getById(initial.food_id, f)) {
            food_ = f;
            hasFood_ = true;
        }
    } else {
        quantity_ = 1;
        note_.clear();
    }
}

void BillItemEditScreen::drawForm() const {
    cls();
    gotoRC(0, 0);

    cout << (isNew_ ? "=== THEM MON VAO HOA DON ===\n\n" : "=== SUA MON TRONG HOA DON ===\n\n");

    cout << "Ban   : " << table_.number << "\n\n";

    cout << (focus_ == 0 ? "> " : "  ");
    cout << "Mon an: " << (hasFood_ ? food_.name : string("(chua chon)")) << "\n";

    cout << (focus_ == 1 ? "> " : "  ");
    cout << "So luong: " << quantity_ << "\n";

    cout << (focus_ == 2 ? "> " : "  ");
    cout << "Ghi chu : " << note_ << "\n\n";

    cout << (focus_ == 3 ? "> " : "  ") << "[Xac nhan]\n\n";

    if (!msg_.empty()) {
        cout << msg_ << "\n\n";
    }

    gotoRC(20, 0);
    clearLine();
    printHint("[Up/Down] chon field  |  [Enter] edit/chon  |  [Backspace] quay lai");
    cout.flush();
}

void BillItemEditScreen::draw() {
    drawForm();
}

void BillItemEditScreen::redrawFocusLines() {
    gotoRC(4, 0);
    clearLine();
    cout << (focus_ == 0 ? "> " : "  ")
         << "Mon an: " << (hasFood_ ? food_.name : string("(chua chon)"));

    gotoRC(5, 0);
    clearLine();
    cout << (focus_ == 1 ? "> " : "  ")
         << "So luong: " << quantity_;

    gotoRC(6, 0);
    clearLine();
    cout << (focus_ == 2 ? "> " : "  ")
         << "Ghi chu : " << note_;

    gotoRC(8, 0);
    clearLine();
    cout << (focus_ == 3 ? "> " : "  ") << "[Xac nhan]";

    cout.flush();
}

void BillItemEditScreen::showMsg() {
    gotoRC(10, 0);
    clearLine();
    if (!msg_.empty())
        cout << msg_;

    gotoRC(20, 0);
    clearLine();
    printHint("[Up/Down] chon field  |  [Enter] edit/chon  |  [Backspace] quay lai");
    cout.flush();
}

void BillItemEditScreen::chooseFood() {
    app->push(new FoodListScreen(app, true, this));
}

void BillItemEditScreen::setFood(const Food &f) {
    food_ = f;
    hasFood_ = true;
    redrawFocusLines();
}

void BillItemEditScreen::editQuantity(Key k) {
    int q = quantity_;
    if (::promptInlineInt(5, "So luong", q)) {
        if (q < 1)
            q = 1;
        quantity_ = q;
    }
    redrawFocusLines();
}

void BillItemEditScreen::editNote(Key k) {
    string s = note_;
    int result = ::promptInlineText(6, "Ghi chu", s);
    if (result == 1) {
        note_ = s;
    }
    redrawFocusLines();
}

void BillItemEditScreen::save() {
    Result r;

    if (isNew_) {
        if (!hasFood_) {
            msg_ = "Vui long chon mon an!";
            showMsg();
            return;
        }
        r = billBLL_.addFoodToTable(table_.id, food_.id, quantity_, note_);
    } else {
        r = billBLL_.updateBillItem(item_.id, quantity_, note_);
    }

    if (r.ok) {
        app->pop();
        return;
    }

    msg_ = r.message;
    showMsg();
}

void BillItemEditScreen::onKey(Key k) {
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
            chooseFood();
            return;
        } else if (focus_ == 1) {
            editQuantity(k);
        } else if (focus_ == 2) {
            editNote(k);
        } else if (focus_ == 3) {
            save();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}