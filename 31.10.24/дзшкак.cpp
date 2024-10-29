#include <iostream>
#include <sqlite_modern_cpp.h>
#include <thread>

class AccountManager {
public:
    static bool sign_in(std::string login, std::string password) {
        try {
            bool check = false;
            db << "select count(*) from users where login = ? and password = ?;"
                << login
                << password
                >> check;

            if (!check)
                return false;
            return true;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    static bool sign_up(std::string name,
        std::string login,
        std::string password) {
        try {
            bool check = false;
            db << "select count(*) from users where login = ?;"
                << login
                >> check;

            if (check)
                return false;

            db << "insert into users (name, login, password) "
                "values (?,?,?);"
                << name
                << login
                << password;

            return true;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

private:
    static sqlite::database db;
};

sqlite::database AccountManager::db("database.db");

class CLIManager {
public:
    void run() {
        start_form();
    }
private:
    void start_form() {
        system("cls");
        std::cout << "Выбери действие\n"
            "1. Регистрация\n"
            "2. Вход\n";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            sign_up_form();
            break;
        case 2:
            sign_in_form();
            break;
        default:
            break;
        }
    }

    void sign_in_form() {
        system("cls");

        std::string login, password;

        std::cout << "Авторизация\n";
        std::cout << "Логин: "; std::cin >> login;
        std::cout << "Пароль: "; std::cin >> password;
        if (AccountManager::sign_in(login, password)) {
            std::cout << "Вход успешно выполнен\n";
        }
        else {
            std::cout << "Неверный логин или пароль!\n";
            start_form();
        };
    }

    void sign_up_form() {
        system("cls");

        std::string name, login, password;

        std::cout << "Регистрация\n";
        std::cout << "Имя: "; std::cin >> name;
        std::cout << "Логин: "; std::cin >> login;
        std::cout << "Пароль: "; std::cin >> password;

        if (AccountManager::sign_up(name, login, password)) {
            std::cout << "Пользователь успешно создан!\n";
            sign_in_form();
        }
        else {
            std::cout << "Пользователь с таким логином уже существует! Попробуйте снова\n";
            start_form();
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru");
    CLIManager cli;
    cli.run();
}