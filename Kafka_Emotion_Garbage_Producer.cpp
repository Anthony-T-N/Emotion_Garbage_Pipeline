#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "simdjson.h"

#include "json.hpp"


//Lessons:
// Mistake 1: Manually copied and pasted raw contents of "simdjson.cpp" into new file named that.
// Mistake 2: Had both #include "simdjson.h" and #include "simdjson.cpp" causing "already defined in obj" errors.

int json_creation()
{
    /**
    simdjson::ondemand::parser parser;
    auto json = simdjson::padded_string::load("sample_profile.json");
    simdjson::ondemand::document doc = parser.iterate(json);
    std::cout << uint64_t(doc["name"]["emotion"]) << " results." << std::endl;
    return 1;


    simdjson::ondemand::parser parser;
    // Opens file.
    simdjson::padded_string json = simdjson::padded_string::load("twitter.json");
    simdjson::ondemand::document tweets = parser.iterate(json);
    uint64_t identifier = tweets["statuses"].at(0)["id"];
    std::cout << identifier << std::endl;
    return EXIT_SUCCESS;
    /*
    std::cout << uint64_t(tweets["search_metadata"]["count"]) << " results." << std::endl;
    */

    std::ifstream f("sample_profile.json");
    nlohmann::json data = nlohmann::json::parse(f);
    std::string s = data.dump();
    std::cout << s << "\n";

    return 1;


}

int main()
{
    json_creation();
    std::vector<std::string> physical_states = { "Moving", "Idle" };
    std::vector<std::string> primary_emotions = { "\033[93mHappy", "\033[36mSad", "\033[91mAnger", "\033[95mFear", "\033[92mDisgust", "\033[96mSurprise" };
    int i = 0;
    while (true)
    {
        i++;
        std::cout << primary_emotions[rand() % primary_emotions.size()] + "\033[0m" << "\n";
        if (i == 20)
        {
            break;
        }
    }
}

/**

Kafka_Emotion_Garbage_Producer -> Kafka Instance -> Consumer

Sample Json
{
    "name": "John Doe",
    "emotion":  "Happy"
    "physical state": "Happy"
    "source":
}

**/
