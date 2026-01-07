// UserUI.cpp
#include "UserUI.h"
#include "AdminUI.h"
#include "ConsoleUtils.h"
#include "SearchSortUtils.h"
#include "StaffUI.h"
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

UserLoginScreen::UserLoginScreen(App *a)
    : Screen(a),
      userBLL_(a->user()),
      username_(""),
      password_(""),
      focus_(0),
      msg_(""),
      userRow_(0),
      passRow_(0),
      actionRow_(0),
      msgRow_(0) {}

void UserLoginScreen::drawForm() const {
    cls();
    printTitleBox("DANG NHAP HE THONG", "Nhap thong tin, nhan Enter de tiep tuc");
    printDivider('=');

    // Dong 3: Username
    userRow_ = contentRow();
    cout << bullet(focus_ == 0) << "Username: " << username_ << "\n";

    // Dong 4: Password (in bang *)
    passRow_ = contentRow();
    cout << bullet(focus_ == 1) << "Password: ";
    for (size_t i = 0; i < password_.size(); ++i)
        cout << '*';
    cout << "\n\n";

    // Dong 6: [Dang nhap]
    actionRow_ = contentRow();
    cout << bullet(focus_ == 2) << "[Dang nhap]\n\n";

    msgRow_ = contentRow();
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_ << "\n\n";
        resetColor();
    }

    printHint("[Up/Down] chon field  |  [Enter] edit/dang nhap  |  [Backspace]/[Esc] thoat");
    cout.flush();
}

void UserLoginScreen::draw() {
    drawForm();
}

void UserLoginScreen::redrawFocusLines() {
    gotoRC(userRow_, 0);
    clearLine();
    cout << bullet(focus_ == 0) << "Username: " << username_;

    gotoRC(passRow_, 0);
    clearLine();
    cout << bullet(focus_ == 1) << "Password: ";
    for (size_t i = 0; i < password_.size(); ++i)
        cout << '*';

    gotoRC(actionRow_, 0);
    clearLine();
    cout << bullet(focus_ == 2) << "[Dang nhap]";

    cout.flush();
}

void UserLoginScreen::showMsg() {
    gotoRC(msgRow_, 0);
    clearLine();
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_;
        resetColor();
    }
    cout.flush();
}

void UserLoginScreen::editUsername() {
    username_.clear();
    string buffer = username_;
    showCursor();

    while (true) {
        gotoRC(userRow_, 0);
        clearLine();
        cout << bullet(focus_ == 0) << "Username: " << buffer;
        cout.flush();

        int ch = _getch();
        if (ch == 13) { // Enter
            username_ = buffer;
            break;
        } else if (ch == 27) { // ESC: huy edit, giu nguyen gia tri cu
            break;
        } else if (ch == 8) { // Backspace xoa 1 ki tu
            if (!buffer.empty())
                buffer.erase(buffer.size() - 1);
        } else if (ch == 0 || ch == 224) {
            _getch();
        } else if (ch >= 32 && ch <= 126) {
            buffer.push_back((char)ch);
        }
    }

    hideCursor();
}

void UserLoginScreen::editPassword() {
    password_.clear();
    string buffer = password_;
    showCursor();

    while (true) {
        gotoRC(passRow_, 0);
        clearLine();
        cout << bullet(focus_ == 1) << "Password: ";
        for (size_t i = 0; i < buffer.size(); ++i)
            cout << '*';
        cout.flush();

        int ch = _getch();
        if (ch == 13) { // Enter
            password_ = buffer;
            break;
        } else if (ch == 27) { // ESC -> huy edit
            break;
        } else if (ch == 8) { // Backspace
            if (!buffer.empty())
                buffer.erase(buffer.size() - 1);
        } else if (ch == 0 || ch == 224) {
            _getch();
        } else if (ch >= 32 && ch <= 126) {
            buffer.push_back((char)ch);
        }
    }

    hideCursor();
}

void UserLoginScreen::doLogin() {
    msg_.clear();
    Result r = userBLL_.login(username_, password_);
    if (!r.ok) {
        msg_ = r.message;
        showMsg();
        drawForm();
        return;
    }

    User u = userBLL_.getCurrentUser();
    // roleId: 1 = Admin, 0 = Nhan vien
    if (u.roleId == 1) {
        // Admin -> vao AdminMenu
        app->replaceTop(new AdminMenuScreen(app));
    } else {
        // Staff -> vao StaffMenu
        app->replaceTop(new StaffMenuScreen(app));
    }
    // App::run() se tu ve man hinh moi, nen khong draw() day nay
}

void UserLoginScreen::onKey(Key k) {
    bool focusChanged = false;

    if (k == KEY_UP) {
        if (focus_ > 0) {
            focus_--;
            focusChanged = true;
        }
    } else if (k == KEY_DOWN) {
        if (focus_ < 2) {
            focus_++;
            focusChanged = true;
        }
    } else if (k == KEY_BACKSPACE || k == KEY_ESC) {
        app->exit();
        return;
    } else if (k == KEY_ENTER) {
        if (focus_ == 0) {
            editUsername();
            focusChanged = true;
        } else if (focus_ == 1) {
            editPassword();
            focusChanged = true;
        } else if (focus_ == 2) {
            doLogin();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}

// ======================= UserProfileScreen =======================
UserProfileScreen::UserProfileScreen(App *a)
    : Screen(a), userBLL_(a->user()) {
    user_ = userBLL_.getCurrentUser();
}

void UserProfileScreen::draw() {
    cls();
    string subtitle = "Nguoi dung: " + user_.userName + " (" + userBLL_.roleToString(user_.roleId) + ")";
    printTitleBox("THONG TIN CA NHAN", subtitle);
    printDivider('=');

    cout << "Ho va ten     : " << user_.fullName << "\n";
    cout << "So dien thoai : " << user_.phoneNumber << "\n";
    cout << "Ngay sinh     : " << user_.birth << "\n";
    cout << "Gioi tinh     : " << userBLL_.genderToString(user_.genderId) << "\n";
    cout << "Vai tro       : " << userBLL_.roleToString(user_.roleId) << "\n\n";

    printHint("[Backspace]/[Enter] Quay lai");
    cout.flush();
}

void UserProfileScreen::onKey(Key k) {
    if (k == KEY_BACKSPACE || k == KEY_ENTER) {
        app->pop();
        return;
    }
}

// ======================= UserListScreen =======================
UserListScreen::UserListScreen(App *a)
    : PagedListScreen<User>(a),
      userBLL_(a->user()),
      selectMode_(false),
      inAction_(false),
      actionIndex_(0),
      msg_(""),
      showingMsg_(false),
      viewNote_(""),
      msgRow_(0),
      filterType_(UserFilterType::None),
      sortField_(UserSortField::None),
      sortAsc_(true),
      keyword_(""),
      filterGender_(-1),
      filterRole_(-1),
      filterBirthYear_(0),
      topActionRow_(3),
      topActionIndex_(0),
      onTopAction_(true) {
    reloadData();
}

void UserListScreen::reloadData() {
    if (filterType_ == UserFilterType::UserName) {
        items_ = userBLL_.searchByUserName(keyword_);
    } else if (filterType_ == UserFilterType::FullName) {
        items_ = userBLL_.searchByFullName(keyword_);
    } else if (filterType_ == UserFilterType::Phone) {
        items_ = userBLL_.searchByPhone(keyword_);
    } else if (filterType_ == UserFilterType::Gender) {
        items_ = userBLL_.searchByGenderId(filterGender_);
    } else if (filterType_ == UserFilterType::BirthYear) {
        items_ = userBLL_.searchByBirthYear(filterBirthYear_);
    } else {
        items_ = userBLL_.getAll();
    }

    applySort();
    calculatePagination();
}

void UserListScreen::applySort() {
    if (sortField_ == UserSortField::None)
        return;

    if (filterType_ == UserFilterType::None) {
        if (sortField_ == UserSortField::UserName) {
            items_ = userBLL_.getAllSortedByUserName(sortAsc_);
        } else if (sortField_ == UserSortField::FullName) {
            items_ = userBLL_.getAllSortedByFullName(sortAsc_);
        } else if (sortField_ == UserSortField::Gender) {
            items_ = userBLL_.getAllSortedByGender(sortAsc_);
        } else if (sortField_ == UserSortField::Birth) {
            items_ = userBLL_.getAllSortedByBirth(sortAsc_);
        }
        return;
    }

    sort(items_.begin(), items_.end(), [&](const User &a, const User &b) {
        if (sortField_ == UserSortField::UserName)
            return sortAsc_ ? a.userName < b.userName : a.userName > b.userName;
        if (sortField_ == UserSortField::FullName)
            return sortAsc_ ? a.fullName < b.fullName : a.fullName > b.fullName;
        if (sortField_ == UserSortField::Gender)
            return sortAsc_ ? a.genderId < b.genderId : a.genderId > b.genderId;
        if (sortField_ == UserSortField::Birth)
            return sortAsc_ ? a.birth < b.birth : a.birth > b.birth;
        return false;
    });
}

void UserListScreen::resetView() {
    filterType_ = UserFilterType::None;
    sortField_ = UserSortField::None;
    sortAsc_ = true;
    keyword_.clear();
    filterGender_ = -1;
    filterRole_ = -1;
    filterBirthYear_ = 0;
    viewNote_.clear();
}

bool UserListScreen::promptStringInput(const string &title, const string &label, string &out) {
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

bool UserListScreen::promptIntInput(const string &title, const string &label, int &out) {
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

bool UserListScreen::promptGender(int &gender) {
    cls();
    printTitleBox("Loc theo gioi tinh");
    printDivider('=');
    cout << "0. Nu\n";
    cout << "1. Nam\n\n";
    printHint("[0]/[1] chon  |  [Backspace]/[Esc] huy");
    cout.flush();

    while (true) {
        int c = _getch();
        if (c == '0') {
            gender = 0;
            return true;
        }
        if (c == '1') {
            gender = 1;
            return true;
        }
        if (c == 27 || c == 8)
            return false;
    }
}

bool UserListScreen::promptSortOrder(bool &asc) {
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

vector<UserListScreen::UserAction> UserListScreen::topActions() const {
    vector<UserAction> acts;
    acts.push_back(UserAction::Add);
    acts.push_back(UserAction::Search);
    acts.push_back(UserAction::Sort);
    return acts;
}

void UserListScreen::drawTopActions() {
    vector<UserAction> actions = topActions();
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
        case UserAction::Add:
            cout << "[Them]";
            break;
        case UserAction::Search:
            cout << "[Tim kiem]";
            break;
        case UserAction::Sort:
            cout << "[Sap xep]";
            break;
        }
    }
    cout.flush();
}

void UserListScreen::handleTopAction(UserAction action) {
    if (action == UserAction::Add) {
        User init;
        init.id = 0;
        init.userName = "";
        init.password = "";
        init.fullName = "";
        init.phoneNumber = "";
        init.birth = "";
        init.genderId = 0;
        init.roleId = 0;
        app->push(new UserEditScreen(app, true, init));
        return;
    }
    if (action == UserAction::Search) {
        openSearchMenu();
        return;
    }
    openSortMenu();
}

void UserListScreen::openSearchMenu() {
    inAction_ = false;
    msg_.clear();

    auto buildFilterLabel = [&]() -> string {
        switch (filterType_) {
        case UserFilterType::UserName:
            return string("Username chua \"") + keyword_ + "\"";
        case UserFilterType::FullName:
            return string("Ho ten chua \"") + keyword_ + "\"";
        case UserFilterType::Phone:
            return string("So dien thoai chua \"") + keyword_ + "\"";
        case UserFilterType::Gender:
            return string("Gioi tinh: ") + userBLL_.genderToString(filterGender_);
        case UserFilterType::BirthYear:
            return string("Nam sinh: ") + to_string(filterBirthYear_);
        default:
            return string();
        }
    };

    auto buildSortLabel = [&]() -> string {
        switch (sortField_) {
        case UserSortField::UserName:
            return string("Sap xep username (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
        case UserSortField::FullName:
            return string("Sap xep ho ten (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
        case UserSortField::Gender:
            return string("Sap xep gioi tinh (") + (sortAsc_ ? "Nu->Nam" : "Nam->Nu") + ")";
        case UserSortField::Birth:
            return string("Sap xep ngay sinh (") + (sortAsc_ ? "cu->tre" : "tre->cu") + ")";
        default:
            return string();
        }
    };

    auto updateViewNote = [&]() {
        string label = buildFilterLabel();
        string sortLabel = buildSortLabel();
        if (!sortLabel.empty()) {
            if (!label.empty())
                label += " | " + sortLabel;
            else
                label = sortLabel;
        }
        viewNote_ = label;
    };

    vector<string> options = {
        "Hien tat ca",
        "Tim theo username",
        "Tim theo ho ten",
        "Tim theo so dien thoai",
        "Loc theo gioi tinh",
        "Loc theo nam sinh"};

    int sel = 0;
    if (filterType_ == UserFilterType::UserName)
        sel = 1;
    else if (filterType_ == UserFilterType::FullName)
        sel = 2;
    else if (filterType_ == UserFilterType::Phone)
        sel = 3;
    else if (filterType_ == UserFilterType::Gender)
        sel = 4;
    else if (filterType_ == UserFilterType::BirthYear)
        sel = 5;

    cls();
    printTitleBox("TIM KIEM NHAN VIEN");
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

    auto promptStringInline = [&](int row, string &value) -> int {
        string caption = options[sel];
        return ::promptInlineText(row, caption, value);
    };

    auto promptIntInline = [&](int row, const string &label, int &value) -> int {
        string caption = options[sel];
        return ::promptInlineInt(row, caption, value);
    };

    auto toggleInline = [&](int row, const vector<pair<string, int>> &choices, int &value) -> bool {
        int idx = 0;
        for (size_t i = 0; i < choices.size(); ++i) {
            if (choices[i].second == value) {
                idx = (int)i;
                break;
            }
        }
        while (true) {
            gotoRC(row, 0);
            clearLine();
            cout << bullet(true) << options[sel] << " : ";
            for (size_t i = 0; i < choices.size(); ++i) {
                if (i > 0)
                    cout << " ";
                cout << (idx == (int)i ? "[*" + choices[i].first + "*]" : "[" + choices[i].first + "]");
            }
            cout.flush();
            Key k2 = readKey();
            if (k2 == KEY_LEFT && idx > 0) {
                idx--;
            } else if (k2 == KEY_RIGHT && idx + 1 < (int)choices.size()) {
                idx++;
            } else if (k2 == KEY_ENTER) {
                value = choices[idx].second;
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
                string kw = keyword_;
                int result = promptStringInline(menuStartRow + sel, kw);
                if (result == 1) {
                    filterType_ = UserFilterType::UserName;
                    keyword_ = kw;
                    changed = true;
                    updateViewNote();
                    break;
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 2) {
                string kw = keyword_;
                int result = promptStringInline(menuStartRow + sel, kw);
                if (result == 1) {
                    filterType_ = UserFilterType::FullName;
                    keyword_ = kw;
                    changed = true;
                    updateViewNote();
                    break;
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 3) {
                string kw = keyword_;
                int result = promptStringInline(menuStartRow + sel, kw);
                if (result == 1) {
                    filterType_ = UserFilterType::Phone;
                    keyword_ = kw;
                    changed = true;
                    updateViewNote();
                    break;
                } else if (result == -1) {
                    break;
                }
            } else if (sel == 4) {
                int g = (filterGender_ < 0) ? 0 : filterGender_;
                vector<pair<string, int>> choices = {{"Nu", 0}, {"Nam", 1}};
                if (toggleInline(menuStartRow + sel, choices, g)) {
                    filterType_ = UserFilterType::Gender;
                    filterGender_ = g;
                    changed = true;
                    updateViewNote();
                    break;
                }
            } else if (sel == 5) {
                int year = filterBirthYear_;
                int result = promptIntInline(menuStartRow + sel, "Nam sinh: ", year);
                if (result == 1) {
                    filterType_ = UserFilterType::BirthYear;
                    filterBirthYear_ = year;
                    changed = true;
                    updateViewNote();
                    break;
                } else if (result == -1) {
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

void UserListScreen::openSortMenu() {
    inAction_ = false;
    msg_.clear();

    auto buildFilterLabel = [&]() -> string {
        switch (filterType_) {
        case UserFilterType::UserName:
            return string("Username chua \"") + keyword_ + "\"";
        case UserFilterType::FullName:
            return string("Ho ten chua \"") + keyword_ + "\"";
        case UserFilterType::Phone:
            return string("So dien thoai chua \"") + keyword_ + "\"";
        case UserFilterType::Gender:
            return string("Gioi tinh: ") + userBLL_.genderToString(filterGender_);
        case UserFilterType::BirthYear:
            return string("Nam sinh: ") + to_string(filterBirthYear_);
        default:
            return string();
        }
    };

    auto buildSortLabel = [&]() -> string {
        switch (sortField_) {
        case UserSortField::UserName:
            return string("Sap xep username (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
        case UserSortField::FullName:
            return string("Sap xep ho ten (") + (sortAsc_ ? "A->Z" : "Z->A") + ")";
        case UserSortField::Gender:
            return string("Sap xep gioi tinh (") + (sortAsc_ ? "Nu->Nam" : "Nam->Nu") + ")";
        case UserSortField::Birth:
            return string("Sap xep ngay sinh (") + (sortAsc_ ? "gia->tre" : "tre->gia") + ")";
        default:
            return string();
        }
    };

    auto updateViewNote = [&]() {
        string label = buildFilterLabel();
        string sortLabel = buildSortLabel();
        if (!sortLabel.empty()) {
            if (!label.empty())
                label += " | " + sortLabel;
            else
                label = sortLabel;
        }
        viewNote_ = label;
    };

    vector<pair<string, UserSortField>> options = {
        /*{"Bo sap xep", UserSortField::None},*/
        {"Username", UserSortField::UserName},
        {"Ho ten", UserSortField::FullName},
        {"Gioi tinh", UserSortField::Gender},
        {"Ngay sinh", UserSortField::Birth}};

    int sel = 0;
    bool asc = sortAsc_;
    for (size_t i = 0; i < options.size(); ++i) {
        if (sortField_ == options[i].second) {
            sel = (int)i;
            break;
        }
    }

    cls();
    printTitleBox("SAP XEP NHAN VIEN");
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
            UserSortField chosen = options[sel].second;
            if (chosen == UserSortField::None) {
                sortField_ = UserSortField::None;
                sortAsc_ = true;
                updateViewNote();
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
                // After breaking from order selection, redraw and continue menu
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

void UserListScreen::drawTableHeader() {
    const int tableWidth = 95;
    printDivider('=', tableWidth);
    setColor(COLOR_ACCENT);
    stringstream hs;
    hs << "  "
       << left << setw(6) << " ID"
       << left << setw(15) << "Username"
       << left << setw(25) << "Ho ten"
       << left << setw(15) << "SDT"
       << left << setw(15) << "Ngay sinh"
       << left << setw(10) << "Gioi tinh";
    cout << "| " << left << setw(tableWidth - 4) << hs.str() << " |\n";
    resetColor();
    printDivider('-', tableWidth);
}

void UserListScreen::drawRowItem(int index, const User &item, bool selected) {
    const int tableWidth = 95;
    int relativeRow = firstRow_ + (index % ITEMS_PER_PAGE);
    gotoRC(relativeRow, 0);
    clearLine();

    bool showArrow = (selected && !inAction_ && !onTopAction_);
    stringstream ss;
    ss << bullet(showArrow)
       << left << setw(6) << item.id
       << left << setw(15) << item.userName
       << left << setw(25) << item.fullName
       << left << setw(15) << item.phoneNumber
       << left << setw(15) << item.birth
       << left << setw(10) << userBLL_.genderToString(item.genderId);
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
}

void UserListScreen::draw() {
    reloadData();
    cls();

    printTitleBox("QUAN LY NHAN SU", "Enter de Them hoac mo menu Sua/Xoa");
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
    if (!inAction_) {
        printHint("[Up/Down] chon  |  [Left/Right] chon Them/Tim/Sap xep  |  [Enter] thuc hien  |  [Backspace] quay lai");
    } else {
        printHint("[Left/Right] chon Sua/Xoa  |  [Enter] thuc hien  |  [Backspace] huy menu");
    }

    msgRow_ = contentRow();
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_;
        resetColor();
    }
    cout.flush();
}

void UserListScreen::onKey(Key k) {
    if (showingMsg_) {
        showingMsg_ = false;
        msg_.clear();
        gotoRC(msgRow_, 0);
        clearLine();
        cout.flush();
    }

    if (onTopAction_) {
        vector<UserAction> actions = topActions();
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
            if (selectedIndex_ >= 0 && selectedIndex_ < (int)items_.size()) {
                inAction_ = true;
                actionIndex_ = 0;
                redrawRow(selectedIndex_, true);
                cout.flush();
            }
        } else if (k == KEY_BACKSPACE) {
            app->pop();
            return;
        }
        return;
    } else {
        int oldAction = actionIndex_;
        if (k == KEY_LEFT) {
            msg_.clear();
            if (actionIndex_ > 0)
                actionIndex_--;
        } else if (k == KEY_RIGHT) {
            msg_.clear();
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
                User u = items_[selectedIndex_];
                if (actionIndex_ == 0) {
                    app->push(new UserEditScreen(app, false, u));
                    inAction_ = false;
                } else {
                    Result r = userBLL_.removeChecked(u.id);
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

// ======================= UserEditScreen =======================
UserEditScreen::UserEditScreen(App *a, bool isNew, const User &initial)
    : Screen(a),
      userBLL_(a->user()),
      isNew_(isNew),
      user_(initial),
      focus_(0),
      msg_(""),
      baseRow_(0) {}

void UserEditScreen::drawForm() const {
    cls();
    gotoRC(0, 0);
    string title = isNew_ ? "THEM NHAN VIEN" : "SUA NHAN VIEN";
    printTitleBox(title);
    printDivider('=');

    baseRow_ = contentRow();

    cout << bullet(focus_ == 0) << "Username : " << user_.userName << "\n";
    cout << bullet(focus_ == 1) << "Password : " << string(user_.password.size(), '*') << "\n";
    cout << bullet(focus_ == 2) << "Ho ten   : " << user_.fullName << "\n";
    cout << bullet(focus_ == 3) << "Phone    : " << user_.phoneNumber << "\n";
    cout << bullet(focus_ == 4) << "Birth    : " << user_.birth << " (yyyy-mm-dd)\n";
    cout << bullet(focus_ == 5) << "Gender   : " << userBLL_.genderToString(user_.genderId) << "  (<- -> de doi)\n";
    cout << bullet(focus_ == 6) << "Role     : " << userBLL_.roleToString(user_.roleId) << " (mac dinh nhan vien)\n\n";

    cout << bullet(focus_ == 7) << "[Xac nhan]\n\n";

    gotoRC(baseRow_ + 10, 0);
    clearLine();
    if (!msg_.empty()) {
        setColor(COLOR_ALERT);
        cout << msg_;
        resetColor();
    }
    gotoRC(22, 0);
    clearLine();
    printHint("[Up/Down] chon field | [Enter] edit | [Backspace] quay lai");
    cout.flush();
}

void UserEditScreen::redrawFocusLines() const {
    gotoRC(baseRow_, 0);
    clearLine();
    cout << bullet(focus_ == 0) << "Username : " << user_.userName;

    gotoRC(baseRow_ + 1, 0);
    clearLine();
    cout << bullet(focus_ == 1) << "Password : " << string(user_.password.size(), '*');

    gotoRC(baseRow_ + 2, 0);
    clearLine();
    cout << bullet(focus_ == 2) << "Ho ten   : " << user_.fullName;

    gotoRC(baseRow_ + 3, 0);
    clearLine();
    cout << bullet(focus_ == 3) << "Phone    : " << user_.phoneNumber;

    gotoRC(baseRow_ + 4, 0);
    clearLine();
    cout << bullet(focus_ == 4) << "Birth    : " << user_.birth << " (yyyy-mm-dd)";

    gotoRC(baseRow_ + 5, 0);
    clearLine();
    cout << bullet(focus_ == 5) << "Gender   : " << userBLL_.genderToString(user_.genderId) << "  (<- -> de doi)";

    gotoRC(baseRow_ + 6, 0);
    clearLine();
    cout << bullet(focus_ == 6) << "Role     : " << userBLL_.roleToString(user_.roleId) << " (mac dinh nhan vien)";

    gotoRC(baseRow_ + 8, 0);
    clearLine();
    cout << bullet(focus_ == 7) << "[Xac nhan]";

    gotoRC(22, 0);
    clearLine();
    printHint("[Up/Down] chon field | [Enter] edit | [Backspace] quay lai");

    cout.flush();
}

void UserEditScreen::editTextField(int focusField, const string &label, string &value, bool mask) {
    string buffer = value;
    showCursor();

    while (true) {
        int row = baseRow_ + focusField;
        gotoRC(row, 0);
        clearLine();
        cout << bullet(focus_ == focusField) << label << (mask ? string(buffer.size(), '*') : buffer);
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (ch == 13) { // Enter
            value = buffer;
            break;
        } else if (ch == 27) { // ESC
            break;
        } else if (ch == 8) { // Backspace
            if (!buffer.empty())
                buffer.pop_back();
        } else if (isprint(static_cast<unsigned char>(ch))) {
            buffer.push_back(static_cast<char>(ch));
        }
    }

    hideCursor();
    redrawFocusLines();
}

static bool isValidDateFormat(const string &s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-')
        return false;
    for (int i : {
             0, 1, 2, 3, 5, 6, 8, 9}) {
        if (!isdigit(static_cast<unsigned char>(s[i])))
            return false;
    }
    return true;
}

void UserEditScreen::editBirth() {
    string buffer = user_.birth;
    if (!isValidDateFormat(buffer))
        buffer = "0000-00-00";

    int cursor = 0;
    showCursor();

    while (true) {
        gotoRC(baseRow_ + 4, 0);
        clearLine();
        cout << bullet(focus_ == 4) << "Birth    : " << buffer << " (yyyy-mm-dd)";
        int col = 12 + cursor;
        gotoRC(baseRow_ + 4, col);
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int c2 = _getch();
            if (c2 == 75) { // left
                if (cursor > 0)
                    cursor--;
                if (cursor == 4 || cursor == 7)
                    cursor--;
            } else if (c2 == 77) { // right
                if (cursor < 9)
                    cursor++;
                if (cursor == 4 || cursor == 7)
                    cursor++;
            }
            continue;
        }
        if (ch == 13) { // Enter accept
            user_.birth = buffer;
            break;
        } else if (ch == 27) { // ESC cancel
            break;
        } else if (isdigit(static_cast<unsigned char>(ch))) {
            buffer[cursor] = static_cast<char>(ch);
            if (cursor < 9) {
                cursor++;
                if (cursor == 4 || cursor == 7)
                    cursor++;
            }
        } else if (ch == 8) { // backspace move left
            if (cursor > 0) {
                cursor--;
                if (cursor == 4 || cursor == 7)
                    cursor--;
                buffer[cursor] = '0';
            }
        }
    }

    hideCursor();
    redrawFocusLines();
}

void UserEditScreen::editGender() {
    int cursor = user_.genderId; // 0 nu,1 nam
    while (true) {
        gotoRC(baseRow_ + 5, 0);
        clearLine();
        cout << bullet(focus_ == 5) << "Gender   : "
             << (cursor == 1 ? "[Nam]" : "Nam") << "  "
             << (cursor == 0 ? "[Nu]" : "Nu")
             << "  (<- -> de doi)";
        cout.flush();

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int c2 = _getch();
            if (c2 == 75 || c2 == 77) { // toggle
                cursor = (cursor == 1) ? 0 : 1;
            }
            continue;
        }
        if (ch == 13) { // Enter accept
            user_.genderId = cursor;
            break;
        } else if (ch == 27) { // ESC cancel
            break;
        }
    }
    redrawFocusLines();
}

void UserEditScreen::save() {
    msg_.clear();
    if (user_.roleId != 0 && user_.roleId != 1)
        user_.roleId = 0;

    Result r = isNew_ ? userBLL_.addChecked(user_) : userBLL_.updateChecked(user_);
    msg_ = r.message;

    // Hiển thị message
    gotoRC(baseRow_ + 10, 0);
    clearLine();
    if (!msg_.empty()) {
        setColor(r.ok ? COLOR_ACCENT : COLOR_ALERT);
        cout << msg_;
        resetColor();
    }
    gotoRC(22, 0);
    clearLine();
    printHint("[Up/Down] chon field | [Enter] edit | [Backspace] quay lai");
    cout.flush();
}

void UserEditScreen::draw() {
    drawForm();
}

void UserEditScreen::onKey(Key k) {
    bool focusChanged = false;

    if (k == KEY_UP && focus_ > 0) {
        focus_--;
        focusChanged = true;
    } else if (k == KEY_DOWN && focus_ < 7) {
        focus_++;
        focusChanged = true;
    } else if (k == KEY_BACKSPACE) {
        app->pop();
        return;
    } else if (k == KEY_ENTER) {
        if (focus_ == 0) {
            editTextField(0, "Username : ", user_.userName);
        } else if (focus_ == 1) {
            editTextField(1, "Password : ", user_.password, true);
        } else if (focus_ == 2) {
            editTextField(2, "Ho ten   : ", user_.fullName);
        } else if (focus_ == 3) {
            editTextField(3, "Phone    : ", user_.phoneNumber);
        } else if (focus_ == 4) {
            editBirth();
        } else if (focus_ == 5) {
            editGender();
        } else if (focus_ == 6) {
            user_.roleId = 0; // lock to staff
            redrawFocusLines();
        } else if (focus_ == 7) {
            save();
            return;
        }
    }

    if (focusChanged) {
        redrawFocusLines();
    }
}
