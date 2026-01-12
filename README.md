# 🍜 Restaurant Management System (Ứng dụng Quản lý Nhà hàng)

**PBL2 - Project Based Learning 2** **Topic:** Object-Oriented Programming (OOP) Application for Restaurant Management.

## 📖 Introduction (Giới thiệu)

This project is a console-based application designed to streamline the operations of a restaurant. It was developed as part of the **PBL2** course at **Danang University of Science and Technology (DUT)**.

Unlike basic management tools, this system applies **Object-Oriented Programming (OOP)** principles and the **3-Layer Architecture** (GUI - BLL - DAL) to ensure code maintainability and scalability. It connects to **SQL Server** for robust data storage.

## 🚀 Features (Chức năng)

Based on the project requirements, the application includes:

* **Table Management:** View table status (Empty/Occupied) and book tables.
* **Order Processing:** Digital menu browsing, adding items to orders, and updating quantities.
* **Staff & Menu Management:** CRUD (Create, Read, Update, Delete) operations for food items and staff information.
* **Automatic Billing:** Calculate total bills automatically.
* **Smart Database Connection:** Automatically detects and connects to SQL Server (Supports both LocalDB and Express versions) without hardcoding credentials.
* **Reporting:** Generate daily revenue reports and order history.

## 🛠️ Technical Details (Kỹ thuật sử dụng)

* **Language:** C++ (Standard C++11/14).
* **Architecture:** 3-Layer Architecture (Mô hình 3 lớp):
    * *GUI (Presentation Layer):* Interface interactions.
    * *BLL (Business Logic Layer):* Handling logic & calculations.
    * *DAL (Data Access Layer):* Communicating with the database.
* **Database:** Microsoft SQL Server.
* **Connectivity:** ODBC Driver 17 for SQL Server.
* **IDE:** Visual Studio Code / Visual Studio 2022.

## 👥 Authors

* **Thi Nguyen Thanh Truc** (Dev & Database Design)
* **DaLay Hoai Linh** (UI Design)
* **Instructor:** *Dr.Truong Ngoc Chau*

## 📝 Usage (Hướng dẫn cài đặt)

To run this project on your local machine:

1.  **Clone the repo:**
    ```bash
    git clone https://github.com/Dalgona0405/PBL2_UngDung_QuanLyNhaHang.git
    ```
2.  **Database Setup:**
    * Open **SQL Server Management Studio (SSMS)**.
    * Restore the database from the provided `.bak` file located in the `data/` folder (or run the script).
    * *Alternative:* Ensure LocalDB is installed if using the portable version.
3.  **Configure Connection:**
    * The system uses an intelligent connection string mechanism. Ensure you have **ODBC Driver 17** installed.
4.  **Run the Application:**
    * Open the project in VS Code.
    * Build and Run the `Main.cpp` file.

---
