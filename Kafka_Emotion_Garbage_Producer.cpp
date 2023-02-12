#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "simdjson.h"

#include "json.hpp"

#pragma warning(disable:4996)

std::vector<std::string> physical_states = { "Moving", "Idle" };
std::vector<std::string> primary_emotions = { "\033[93mHappy", "\033[36mSad", "\033[91mAnger", "\033[95mFear", "\033[92mDisgust", "\033[96mSurprise" };

//Lessons:
// Mistake 1: Manually copied and pasted raw contents of "simdjson.cpp" into new file named that.
// Mistake 2: Had both #include "simdjson.h" and #include "simdjson.cpp" causing "already defined in obj" errors.

std::string get_current_date()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    // Error C4996 'localtime': This function or variable may be unsafe.Consider using localtime_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return buffer;
}


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

    std::vector<std::string> loaded_name_vector;
    std::ifstream input_file;
    std::cout << "[!] Opening first-names.txt for reading;" << "\n";
    if (std::filesystem::exists("first-names.txt") == false)
    {
        std::cout << "\033[4;31m" << "[-] Unable to open first-names.txt;" << "\033[0m" << "\n";
        return 1;
    }
    input_file.open("first-names.txt");
    std::cout << "[+] Opened first-names.txt successfully;" << "\n\n";

    // Store as vector or read again.
    std::string input_file_line;
    int line_cout = 0;
    while (std::getline(input_file, input_file_line))
    {
        loaded_name_vector.push_back(input_file_line);
        line_cout++;
    }

    nlohmann::json new_json;
    new_json["time"] = get_current_date();
    new_json["name"] = loaded_name_vector[rand() % line_cout];
    new_json["emotion"] = primary_emotions[rand() % primary_emotions.size()] + "\033[0m";
    new_json["physical state"] = physical_states[rand() % physical_states.size()];
    new_json["source"] = "Device" + std::to_string(rand() % 100);
    std::cout << new_json << "\n";

    std::ifstream loaded_file("sample_profile.json");
    nlohmann::json parsed_file = nlohmann::json::parse(loaded_file);
    std::cout << parsed_file.dump() << "\n";

    return 1;
}

int producer()
{
    std::string broker_ip = "1.1.1.1";
    return 1;
}

int main()
{
    json_creation();
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
    "physical state": "Idle"
    "source":
}

**/
