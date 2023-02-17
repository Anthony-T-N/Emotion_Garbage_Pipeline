#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include <kafka/KafkaProducer.h>

//#include "simdjson.h"

#include "json.hpp"

#pragma warning(disable:4996)

std::vector<std::string> physical_states = { "Moving", "Idle" };
//std::vector<std::string> primary_emotions = { "\033[93mHappy", "\033[36mSad", "\033[91mAnger", "\033[95mFear", "\033[92mDisgust", "\033[96mSurprise" };
std::vector<std::string> primary_emotions = { "Happy", "Sad", "Anger", "Fear", "Disgust", "Surprise" };

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
    strftime(buffer, sizeof(buffer), "%Y-%m-%d.T%H-%M-%S", timeinfo);
    return buffer;
}

nlohmann::json json_creation()
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

    // Store as vector or read again.
    std::string input_file_line;
    int line_cout = 0;
    while (std::getline(input_file, input_file_line))
    {
        loaded_name_vector.push_back(input_file_line);
        line_cout++;
    }

    // Fields
    nlohmann::json new_json;
    new_json["time"] = get_current_date();
    new_json["name"] = loaded_name_vector[rand() % line_cout];
    new_json["emotion"] = primary_emotions[rand() % primary_emotions.size()];
    new_json["physical state"] = physical_states[rand() % physical_states.size()];
    new_json["source"] = "Device" + std::to_string(rand() % 100);
    std::cout << new_json << "\n";

    std::ofstream file(get_current_date() + "-emotion_event.json");
    //file << new_json;

    /*
    std::ifstream loaded_file("sample_profile.json");
    nlohmann::json parsed_file = nlohmann::json::parse(loaded_file);
    std::cout << parsed_file.dump() << "\n";
    */
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return new_json;
}

int json_publish()
{
    /*
    Read current directory to locate files containing the ".json" file extension. When located, parse file using json library
    to identify the key "emotion" and check whether the value of key is "Happy". If so, run command to use script to publish 
    json file to Kafka broker under the topic "emotion_happy". 
    */
    std::string current_json_path = "";
    std::string current_filepath = std::filesystem::current_path().string();
    for (const auto& entry : std::filesystem::directory_iterator(current_filepath))
    {
        current_json_path = "";
        std::string command = "";
        std::cout << entry.path() << std::endl;
        if (entry.path().string().find(".json") != std::string::npos)
        {
            current_json_path = entry.path().string();
            std::cout << current_json_path << "\n";
            std::ifstream loaded_file(current_json_path);
            nlohmann::json parsed_file = nlohmann::json::parse(loaded_file);
            std::cout << parsed_file.at("emotion") << '\n';
            if (parsed_file.at("emotion") == "Happy")
            {
                std::cout << "Key: Happy" << "\n";
                command = "bin/kafka-console-producer.sh --broker-list localhost:9092 --topic emotion_happy < " 
                    + entry.path().string();
            }
            else if (parsed_file.at("emotion") == "Sad")
            {
                std::cout << "Key: Sad" << "\n";
                command = "bin/kafka-console-producer.sh --broker-list localhost:9092 --topic emotion_sad < "
                    + entry.path().string();
            }
            else
            {
                std::cout << "Not an emotion of interest" << "\n";
                command = "bin/kafka-console-producer.sh --broker-list localhost:9092 --topic emotion_irrelevant < "
                    + entry.path().string();
            }
            system("echo \"Sanity Check\"");
            system(command.c_str());
        }
        // Strange bug that can only be resolved by repeating identical IF statment.
        // Sharing violation error 
        if (entry.path().string().find(".json") != std::string::npos)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            //std::filesystem::remove(entry.path().string());
            command = "del " + entry.path().string().substr(entry.path().string().find_last_of("/\\") + 1);
            std::cout << "[-] Deleting: " + command << "\n";
            system(command.c_str());
            std::cout << "\n";
        }
    }
    return 1;
}

int producer(nlohmann::json json_record)
{
    using namespace kafka;
    using namespace kafka::clients::producer;

    std::string broker_ip = "192.168.1.1:9092";

    const std::string broker_ip = getenv("KAFKA_BROKER_LIST");
    const Topic topic = getenv("TEST_TOPIC");

    const Properties props({ {"bootstrap.servers", broker_ip} });

    KafkaProducer producer(props);

    // Convert JSON to record for producer.
    ProducerRecord record(topic, NullKey, Value(json_record.c_str(), json_record.size()));

    auto deliveryCb = [](const RecordMetadata& metadata, const Error& error) {
        if (!error) {
            std::cout << "Message delivered: " << metadata.toString() << std::endl;
        }
        else {
            std::cerr << "Message failed to be delivered: " << error.message() << std::endl;
        }
    };
}

int main()
{
    std::cout << "=======================================" << "\n";
    std::cout << "- Kafka_Emotion_Garbage_Producer console application" << "\n";
    std::cout << "- Console Application Version: 1.0" << "\n";
    std::cout << "- Created By: Anthony-T-N." << "\n";
    std::cout << "- Current location of executable: " << std::filesystem::current_path() << "\n";
    std::cout << "=======================================" << "\n\n";

    //json_publish();
    producer(json_creation());
    
    /*
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
    */
    std::cout << "[!] END" << "\n";
    std::cout << "[!] Exiting..." << "\n\n";
    system("pause");
}

/**

Kafka_Emotion_Garbage_Producer -> Kafka Instance -> Consumer

Sample Json
{
    "name": "John Doe",
    "emotion":  "Happy"
    "physical state": "Idle"
    "source": "Device02"
}

- Created Json -> SFTP to testing enviroment -> Send Json to Kafka broker.


// Create topics for Kafka broker.
$ bin/kafka-topics.sh --create --topic emotion_happy --bootstrap-server localhost:9092
$ bin/kafka-topics.sh --create --topic emotion_sad --bootstrap-server localhost:9092

**/
