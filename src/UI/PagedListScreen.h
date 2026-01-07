#pragma once
#ifndef PAGED_LIST_SCREEN_H
#define PAGED_LIST_SCREEN_H

#include "ConsoleUtils.h"
#include "ConsoleUI.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

template <typename T>
class PagedListScreen : public Screen {
	protected:
		vector<T> items_;
		int currentPage_;
		int totalPages_;
		int selectedIndex_;
		const int ITEMS_PER_PAGE = 7;
		int firstRow_; // Lưu vị trí dòng bắt đầu vẽ list

	public:
		PagedListScreen(App* a) : Screen(a), currentPage_(1), totalPages_(1), selectedIndex_(0), firstRow_(0) {}
		virtual ~PagedListScreen() {}

		virtual void reloadData() = 0;
		virtual void drawTableHeader() = 0;
		virtual void drawRowItem(int index, const T& item, bool selected) = 0;

		void calculatePagination() {
			if (items_.empty()) {
				totalPages_ = 1;
				currentPage_ = 1;
				selectedIndex_ = -1;
			} else {
				totalPages_ = ((int)items_.size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
				if (currentPage_ < 1) currentPage_ = 1;
				if (currentPage_ > totalPages_) currentPage_ = totalPages_;

				if (selectedIndex_ >= 0) {
					// Đảm bảo index hợp lệ
					if(selectedIndex_ >= (int)items_.size()) selectedIndex_ = (int)items_.size() - 1;
					currentPage_ = (selectedIndex_ / ITEMS_PER_PAGE) + 1;
				} else {
					selectedIndex_ = 0;
				}
			}
		}

		void drawList() {
			drawTableHeader();
			firstRow_ = contentRow(); // Lưu lại vị trí này để redrawRow dùng

			int startIdx = (currentPage_ - 1) * ITEMS_PER_PAGE;
			int endIdx = min((int)items_.size(), startIdx + ITEMS_PER_PAGE);

			for (int i = startIdx; i < endIdx; ++i) {
				drawRowItem(i, items_[i], (i == selectedIndex_));
			}

			// Vẽ dòng trống để footer không bị nhảy
			int drawnCount = endIdx - startIdx;
			for(int k = drawnCount; k < ITEMS_PER_PAGE; k++) {
				int emptyRow = firstRow_ + k;
				gotoRC(emptyRow, 0);
				clearLine();
			}
			
			int footerRow = firstRow_ + ITEMS_PER_PAGE + 1;
			gotoRC(footerRow, 0);
			clearLine();
			setColor(COLOR_ACCENT);
			if(items_.empty()) cout << "(Danh sach trong)";
			else cout << "Trang " << currentPage_ << "/" << totalPages_ << " | Tong: " << items_.size();
			resetColor();
		}

		// Hàm mới: Chỉ vẽ lại 1 dòng cụ thể (Không xóa màn hình)
		void redrawRow(int index, bool selected) {
			// Kiểm tra index có thuộc trang hiện tại không
			int startIdx = (currentPage_ - 1) * ITEMS_PER_PAGE;
			int endIdx = startIdx + ITEMS_PER_PAGE;

			if (index >= startIdx && index < endIdx && index < (int)items_.size()) {
				drawRowItem(index, items_[index], selected);
			}
		}

		// Trả về:
		// 0: Không xử lý
		// 1: Đã xử lý di chuyển (tự vẽ lại dòng), không cần vẽ lại full
		// 2: Đã chuyển trang (cần vẽ lại full màn hình)
		int handleNavigationKey(Key k) {
			if (items_.empty()) return 0;

			int oldIndex = selectedIndex_;

			if (k == KEY_DOWN) {
				if (selectedIndex_ + 1 < (int)items_.size()) {
					selectedIndex_++;
					// Kiểm tra xem có sang trang mới không
					int newPage = (selectedIndex_ / ITEMS_PER_PAGE) + 1;
					if (newPage != currentPage_) {
						calculatePagination();
						return 2; // Cần vẽ lại full
					} else {
						// Cùng trang -> Chỉ vẽ lại 2 dòng
						redrawRow(oldIndex, false);     // Bỏ chọn dòng cũ
						redrawRow(selectedIndex_, true); // Chọn dòng mới
						return 1; // Đã xử lý xong, không cần draw()
					}
				}
			} else if (k == KEY_UP) {
				if (selectedIndex_ > 0) {
					selectedIndex_--;
					int newPage = (selectedIndex_ / ITEMS_PER_PAGE) + 1;
					if (newPage != currentPage_) {
						calculatePagination();
						return 2;
					} else {
						redrawRow(oldIndex, false);
						redrawRow(selectedIndex_, true);
						return 1;
					}
				}
			} else if (k == KEY_LEFT) {
				if (currentPage_ > 1) {
					currentPage_--;
					selectedIndex_ = (currentPage_ - 1) * ITEMS_PER_PAGE;
					return 2; // Chuyển trang -> Vẽ lại full
				}
			} else if (k == KEY_RIGHT) {
				if (currentPage_ < totalPages_) {
					currentPage_++;
					selectedIndex_ = (currentPage_ - 1) * ITEMS_PER_PAGE;
					return 2; // Chuyển trang -> Vẽ lại full
				}
			}
			return 0;
		}
};

#endif