#include <iostream>
#include <tgbot/tgbot.h>
#include <sqlite_modern_cpp.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;
using namespace TgBot;
using namespace sqlite;

struct Country {
    std::string name;
    std::string Url;
};

void createDatabase(const string& dbFile) {
    database db(dbFile);
    db << "CREATE TABLE IF NOT EXISTS Countries ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "flagUrl TEXT NOT NULL);";


    db << "INSERT INTO Countries (name, flagUrl) VALUES ('Индия', 'https://ak4.picdn.net/shutterstock/videos/27353794/thumb/1.jpg'), "
        "('Бразилия', 'https://avatars.mds.yandex.net/i?id=30a5983cd610ab2dc3bc253551c9a4b012edfdcc-5332439-images-thumbs&n=13'), "
        "('Франция', 'https://podcasts-francais.fr/img/cover/fr.jpg'), "
        "('Россия', 'https://avatars.mds.yandex.net/i?id=4c4f7a38b58089fef372d5c1139cc090_l-5235076-images-thumbs&n=13') "
        "ON CONFLICT (name) DO NOTHING;";
}

Country getRandomCountry(database& db) {
    Country country;
    db << "SELECT name, flagUrl FROM Countries ORDER BY RANDOM() LIMIT 1;" >>
        [&](string name, string flagUrl) {
        country.name = name;
        country.Url = flagUrl;
        };
    return country;
}

int main() {
    setlocale(LC_ALL, "rus");
    const string dbFile = "countries.db";
    createDatabase(dbFile);

    database db(dbFile);
    Bot bot("7614640276:AAE5L9DbM8YqvimlWxSVfcAITCDQtxjDvbE");

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Welcome! Type /random to get a random country.");
        });

    bot.getEvents().onCommand("random", [&bot, &db](Message::Ptr message) {
        Country country = getRandomCountry(db);

        vector<string> options = { country.name, "Бразилия", "Франция", "Россия" };
        random_shuffle(options.begin(), options.end());

        bot.getApi().sendPhoto(message->chat->id, country.Url, "Угадайте страну!", false, options);
        });

    bot.getEvents().onCommand("help", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Available commands:\n/start - Start the bot\n/random - Get a random country");
        });


    bot.getEvents().onCallbackQuery([&bot, &db](CallbackQuery::Ptr query) {
        Country correctCountry = getRandomCountry(db); 

        if (query->data == correctCountry.name) {
            bot.getApi().sendMessage(query->message->chat->id, "Правильно!");
        }
        else {
            bot.getApi().sendMessage(query->message->chat->id, "Неправильно. Правильный ответ: " + correctCountry.name);
        }
        });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e) {
        printf("error: %s\n", e.what());
    }
}
