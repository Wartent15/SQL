#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <tgbot/tgbot.h>

const std::string TOKEN = "YOUR_TOKEN_HERE";

struct Pokemon {
    std::string name;
    int weight;
    int height;
    std::string photo;
};

class PokemonAPI {
public:
    static nlohmann::json get_response(const std::string& pokemon_name) {
        cpr::Url url{ "https://pokeapi.co/api/v2/pokemon/" + pokemon_name };
        cpr::Response response = cpr::Get(url);
        return nlohmann::json::parse(response.text);
    }
};

class PokemonBuilder {
public:
    static Pokemon from_json(const nlohmann::json& json_pokemon) {
        Pokemon pokemon;
        pokemon.name = json_pokemon["name"];
        pokemon.weight = json_pokemon["weight"];
        pokemon.height = json_pokemon["height"];
        pokemon.photo = json_pokemon["sprites"]["other"]["official-artwork"]["front_default"];
        return pokemon;
    }

    static std::string get_description(const Pokemon& pokemon) {
        return "Name: " + pokemon.name +
            "\nWeight: " + std::to_string(pokemon.weight) +
            "\nHeight: " + std::to_string(pokemon.height);
    }
};

class PokemonListManager {
    std::vector<std::string> pokemon_list;

public:
    PokemonListManager() {
        fetch_pokemon_list();
    }

    void fetch_pokemon_list() {
        cpr::Response response = cpr::Get(cpr::Url{ "https://pokeapi.co/api/v2/pokemon?limit=20&offset=0" });
        nlohmann::json json_response = nlohmann::json::parse(response.text);
        for (auto& item : json_response["results"]) {
            pokemon_list.push_back(item["name"]);
        }
    }

    std::string get_pokemon_list() const {
        std::string pokemons;
        for (const auto& item : pokemon_list)
            pokemons += "/" + item + "\n";
        return pokemons;
    }
};

class PokemonManager {
public:
    static void send_pokemon(TgBot::Bot& bot, int chat_id, const std::string& pokemon_name) {
        Pokemon pokemon = PokemonBuilder::from_json(PokemonAPI::get_response(pokemon_name));
        bot.getApi().sendPhoto(chat_id, pokemon.photo, PokemonBuilder::get_description(pokemon));
    }
};

int main() {
    TgBot::Bot bot(TOKEN);

    bot.getEvents().onCommand("menu", [&](TgBot::Message::Ptr message) {
        PokemonListManager pokemon_list_manager;
        bot.getApi().sendMessage(message->chat->id,
            pokemon_list_manager.get_pokemon_list());
        });

    bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        if (!StringTools::startsWith(message->text, "/") || StringTools::startsWith(message->text, "/menu"))
            return;
        PokemonManager::send_pokemon(bot, message->chat->id, message->text.substr(1)); // Удаляем '/'
        });

    try {
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
}
