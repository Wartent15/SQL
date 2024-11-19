#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h> // Используйте библиотеку JSON для удобного парсинга ответа

// Функция для обработки данных ответа
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция для отправки запроса и получения ответа в виде строки
std::string SendRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Ошибка curl: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

// Функция для поиска книги по названию
void SearchBookByTitle(const std::string& title) {
    std::string url = "https://openlibrary.org/search.json?title=" + title;
    std::string response = SendRequest(url);

    Json::Value jsonData;
    Json::Reader reader;
    if (reader.parse(response, jsonData)) {
        if (jsonData["docs"].size() > 0) {
            std::cout << "Результаты поиска по названию: " << title << std::endl;
            for (const auto& book : jsonData["docs"]) {
                std::cout << "Название: " << book["title"].asString() << std::endl;
                if (book.isMember("author_name")) {
                    std::cout << "Автор(ы): ";
                    for (const auto& author : book["author_name"]) {
                        std::cout << author.asString() << "; ";
                    }
                    std::cout << std::endl;
                }
                if (book.isMember("isbn")) {
                    std::cout << "ISBN: " << book["isbn"][0].asString() << std::endl;
                }
                std::cout << "---------------------------" << std::endl;
            }
        }
        else {
            std::cout << "Книги с таким названием не найдены." << std::endl;
        }
    }
    else {
        std::cerr << "Ошибка парсинга JSON." << std::endl;
    }
}

// Функция для получения информации о книге по ID
void GetBookByID(const std::string& id) {
    std::string url = "https://openlibrary.org/works/" + id + ".json";
    std::string response = SendRequest(url);

    Json::Value jsonData;
    Json::Reader reader;
    if (reader.parse(response, jsonData)) {
        std::cout << "Информация о книге с ID: " << id << std::endl;
        std::cout << "Название: " << jsonData["title"].asString() << std::endl;
        if (jsonData.isMember("description")) {
            std::cout << "Описание: " << jsonData["description"].asString() << std::endl;
        }
        if (jsonData.isMember("authors")) {
            std::cout << "Автор(ы): ";
            for (const auto& author : jsonData["authors"]) {
                std::cout << author["name"].asString() << "; ";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cerr << "Ошибка парсинга JSON." << std::endl;
    }
}

int main() {
    std::string choice;
    while (true) {
        std::cout << "Введите 1 для поиска книги по названию, 2 для поиска по ID или 'exit' для выхода: ";
        std::cin >> choice;

        if (choice == "1") {
            std::string title;
            std::cout << "Введите название книги: ";
            std::cin.ignore();
            std::getline(std::cin, title);
            SearchBookByTitle(title);
        }
        else if (choice == "2") {
            std::string id;
            std::cout << "Введите уникальный идентификатор книги (ID): ";
            std::cin >> id;
            GetBookByID(id);
        }
        else if (choice == "exit") {
            break;
        }
        else {
            std::cout << "Неверный выбор. Пожалуйста, попробуйте снова." << std::endl;
        }
    }

    return 0;
}
