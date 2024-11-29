#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

string performRequest(const string& url, const string& apiKey) {
    CURL* curl;
    CURLcode res;
    string response;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        string header = "Authorization: Bearer " + apiKey;
        headers = curl_slist_append(headers, header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Ошибка HTTP-запроса: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return response;
}

int main() {
    setlocale(LC_ALL, "Russian");
    string apiKey = "https://www.dnd5eapi.co/api/rules/"; 
    string userAddress, category;
    int radius;

    cout << "Введите адрес: ";
    getline(cin, userAddress);

    cout << "Введите категорию (например, кафе, ресторан, автосервис): ";
    getline(cin, category);

    cout << "Введите радиус поиска в метрах (например, 5000): ";
    cin >> radius;

    string geocodeUrl = "https://catalog.api.2gis.com/3.0/geocode?q=" + userAddress + "&key=" + apiKey;
    string geocodeResponse = performRequest(geocodeUrl, apiKey);

    try {
        auto geocodeJson = json::parse(geocodeResponse);
        if (geocodeJson["result"]["items"].empty()) {
            cout << "Адрес не найден. Попробуйте снова." << endl;
            return 1;
        }

        auto coordinates = geocodeJson["result"]["items"][0]["geometry"]["centroid"];
        double lat = coordinates["lat"];
        double lon = coordinates["lon"];
        cout << "Координаты: " << lat << ", " << lon << endl;

        string searchUrl = "https://catalog.api.2gis.com/3.0/items?q=" + category +
            "&point=" + to_string(lon) + "," + to_string(lat) +
            "&radius=" + to_string(radius) + "&key=" + apiKey;

        string searchResponse = performRequest(searchUrl, apiKey);
        auto searchJson = json::parse(searchResponse);

        if (searchJson["result"]["items"].empty()) {
            cout << "Объекты не найдены." << endl;
        }
        else {
            cout << "Найденные объекты:" << endl;
            for (const auto& item : searchJson["result"]["items"]) {
                string name = item["name"];
                string address = item["address_name"];
                cout << "- " << name << " (Адрес: " << address << ")" << endl;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка обработки данных: " << e.what() << endl;
        return 1;
    }
}
