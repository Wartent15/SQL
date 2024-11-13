#include <iostream>
#include <string>
#include <optional>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <sqlite_modern_cpp.h>

class Database {
public:
    Database(const std::string& db_name) : db(db_name) {
        db << "CREATE TABLE IF NOT EXISTS rules ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "rule_index TEXT UNIQUE, "
            "data TEXT);";
    }

    std::optional<std::string> getRuleByIndex(const std::string& index) {
        std::optional<std::string> result;
        db << "SELECT data FROM rules WHERE rule_index = ?;"
            << index
            >> [&](std::string data) {
            result = data;
            };
        return result;
    }

    void saveRule(const std::string& index, const std::string& data) {
        db << "INSERT OR IGNORE INTO rules (rule_index, data) VALUES (?, ?);"
            << index
            << data;
    }

private:
    sqlite::database db;
};

class DnD_API {
public:
    std::optional<nlohmann::json> getRuleByIndex(const std::string& index) {
        std::string url = base_url + index;
        cpr::Response response = cpr::Get(cpr::Url{ url });

        if (response.status_code == 200) {
            return nlohmann::json::parse(response.text);
        }
        else {
            return std::nullopt;
        }
    }

private:
    const std::string base_url = "https://www.dnd5eapi.co/api/rules/";
};

void displayMenu() {
    std::cout << "Dungeons & Dragons Rules Viewer\n";
    std::cout << "1. View rule by index\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

void handleViewRule(Database& db, DnD_API& api) {
    std::string rule_index;
    std::cout << "Enter the rule index (e.g., 'combat', 'equipment', etc.): ";
    std::cin >> rule_index;

    auto cached_rule = db.getRuleByIndex(rule_index);
    if (cached_rule) {
        std::cout << "Rule from cache:\n";
        std::cout << *cached_rule << "\n";
    }
    else {
        auto rule = api.getRuleByIndex(rule_index);
        if (rule) {
            std::cout << "Rule from API:\n";
            std::cout << rule->dump(4) << "\n";

            db.saveRule(rule_index, rule->dump());
        }
        else {
            std::cout << "Failed to retrieve the rule.\n";
        }
    }
}

int main() {
    Database db("dnd_rules.db");
    DnD_API api;

    while (true) {
        displayMenu();
        int choice;
        std::cin >> choice;

        if (choice == 0) break;

        switch (choice) {
        case 1:
            handleViewRule(db, api);
            break;
        default:
            std::cout << "Invalid choice, try again.\n";
            break;
        }
    }

}
