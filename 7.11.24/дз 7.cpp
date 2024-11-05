#include <iostream>
#include <string>
#include <sqlite_modern_cpp.h>
#include <thread>
#include <chrono>

class Database {
public:
    Database(const std::string& dbPath) : db(dbPath) {
        createUsersTable();
    }

    bool registerUser(const std::string& username, const std::string& login, const std::string& password) {
        try {
            db << "INSERT INTO Users (Username, Login, Password) VALUES (?, ?, ?);"
                << username << login << password;
            return true;
        }
        catch (const sqlite::sqlite_exception& e) {
            std::cerr << "Ошибка регистрации: " << e.what() << std::endl;
            return false;
        }
    }

    bool loginUser(const std::string& login, const std::string& password) {
        try {
            int count = 0;
            db << "SELECT COUNT(*) FROM Users WHERE Login = ? AND Password = ?;"
                << login << password >> count;
            return count > 0;
        }
        catch (const sqlite::sqlite_exception& e) {
            std::cerr << "Ошибка авторизации: " << e.what() << std::endl;
            return false;
        }
    }

    bool updateUsername(const std::string& oldUsername, const std::string& newUsername) {
        try {
            db << "UPDATE Users SET Username = ? WHERE Username = ?;"
                << newUsername << oldUsername;
            return true;
        }
        catch (const sqlite::sqlite_exception& e) {
            std::cerr << "Ошибка изменения имени пользователя: " << e.what() << std::endl;
            return false;
        }
    }
    bool updatePassword(const std::string& oldPassword, const std::string& newPassword) {
        try {
            db << "UPDATE Users SET Password = ? WHERE Password = ?;"
                << newPassword << oldPassword;
            return true;
        }
        catch (const sqlite::sqlite_exception& e) {
            std::cerr << "Ошибка изменения пароля пользователя: " << e.what() << std::endl;
            return false;
        }
    }

private:
    void createUsersTable() {
        db << "CREATE TABLE IF NOT EXISTS Users ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "Username TEXT NOT NULL UNIQUE, "
            "Login TEXT NOT NULL UNIQUE, "
            "Password TEXT NOT NULL);";
    }

    sqlite::database db;
};

class Game {
public:
    Game() : db("game.db") {}

    void start() {
        int choice;
        while (true) {
            std::cout << "1. Зарегистрироваться\n2. Войти\nВыберите действие: ";
            std::cin >> choice;

            if (choice == 1) {
                registerUser();
            }
            else if (choice == 2) {
                if (loginUser()) {
                    mainMenu();
                    break;
                }
            }
        }
    }

private:
    void registerUser() {
        system("cls");
        std::string username, login, password;
        std::cout << "Введите имя пользователя: ";
        std::cin >> username;
        std::cout << "Введите логин: ";
        std::cin >> login;
        std::cout << "Введите пароль: ";
        std::cin >> password;

        if (db.registerUser(username, login, password)) {
            std::cout << "Регистрация успешна. Вход выполнен.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else {
            std::cout << "Ошибка регистрации.\n";
        }
    }

    bool loginUser() {
        system("cls");
        std::string login, password;
        std::cout << "Введите логин: ";
        std::cin >> login;
        std::cout << "Введите пароль: ";
        std::cin >> password;

        if (db.loginUser(login, password)) {
            std::cout << "Вход успешен.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return true;
        }
        else {
            std::cout << "Неправильный логин или пароль.\n";
            return false;
        }
    }

    void mainMenu() {
        system("cls");
        int choice;
        while (true) {
            std::cout << "\nГлавное меню:\n";
            std::cout << "1. Начать игру\n";
            std::cout << "2. Просмотреть статистику\n";
            std::cout << "3. Настройки аккаунта\n";
            std::cout << "4. Выйти\n";
            std::cout << "Выберите действие: ";
            std::cin >> choice;

            switch (choice) {
            case 1:
                startGame();
                break;
            case 2:
                showStats();
                break;
            case 3:
                settings();
                break;
            case 4:
                return;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }
    void settings() {
        system("cls");
        std::cout << "Выбери действие\n"
            "1. Изменить пароль\n"
            "2. Изменить имя\n"
            "3. Выход\n";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            change_password();
            break;
        case 2:
            change_name();
            break;
        case 3:
            system("cls");
            mainMenu();
            break;
        default:
            break;
        }
    }

    void startGame() {
        system("cls");
        std::cout << "Начало игры...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void showStats() {
        system("cls");
        std::cout << "Пока нет статистики.\n";
    }
    void change_password() {
        system("cls");
        std::string oldPassword, newPassword;
        std::cout << "Введите текущий пароль  пользователя: ";
        std::cin >> oldPassword;
        std::cout << "Введите новый пароль пользователя: ";
        std::cin >> newPassword;

        if (db.updateUsername(oldPassword, newPassword)) {
            std::cout << "Пароль пользователя успешно изменено.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else {
            std::cout << "Ошибка при изменении пароля пользователя.\n";
        }
    }


    void change_name() {
        system("cls");
        std::string oldUsername, newUsername;
        std::cout << "Введите текущее имя пользователя: ";
        std::cin >> oldUsername;
        std::cout << "Введите новое имя пользователя: ";
        std::cin >> newUsername;

        if (db.updateUsername(oldUsername, newUsername)) {
            std::cout << "Имя пользователя успешно изменено.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else {
            std::cout << "Ошибка при изменении имени пользователя.\n";
        }
    }

    Database db;
};

int main() {
    setlocale(LC_ALL, "ru");
    Game game;
    game.start();
}