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

std::vector<std::string> movement_states = { "Moving", "Idle" };
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

nlohmann::json emotion_event_creation()
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
    nlohmann::json emotion_event;
    emotion_event["time"] = get_current_date();
    emotion_event["name"] = loaded_name_vector[rand() & 0x10 % line_cout];
    emotion_event["emotion"] = primary_emotions[rand() & 0x10 % primary_emotions.size()];
    emotion_event["physical state"] = movement_states[rand() & 0x10 % movement_states.size()];
    emotion_event["source"] = "Device" + std::to_string(rand() & 0x10 % 100);
    std::cout << emotion_event << "\n";

    std::ofstream file(get_current_date() + "-emotion_event.json");
    // File can be created if required.
    //file << emotion_event;

    /*
    std::ifstream loaded_file("sample_profile.json");
    nlohmann::json parsed_file = nlohmann::json::parse(loaded_file);
    std::cout << parsed_file.dump() << "\n";
    */
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return emotion_event;
}

void json_publish()
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
        std::string publish_command = "";
        std::cout << entry.path() << std::endl;
        if (entry.path().string().find(".json") != std::string::npos)
        {
            current_json_path = entry.path().string();
            std::cout << current_json_path << "\n";
            std::ifstream loaded_file(current_json_path);
            nlohmann::json parsed_file = nlohmann::json::parse(loaded_file);
            std::cout << parsed_file.at("emotion") << '\n';
            publish_command = "bin/kafka-console-producer.sh --broker-list localhost:9092 --topic ";
            if (parsed_file.at("emotion") == "Happy")
            {
                std::cout << "Key: Happy" << "\n";
                publish_command += "emotion_happy < "
                    + entry.path().string();
            }
            else if (parsed_file.at("emotion") == "Sad")
            {
                std::cout << "Key: Sad" << "\n";
                publish_command += "emotion_sad < "
                    + entry.path().string();
            }
            else
            {
                std::cout << "Not an emotion of interest" << "\n";
                publish_command += "emotion_irrelevant < "
                    + entry.path().string();
            }
            system("echo \"Sanity Check\"");
            system(publish_command.c_str());
        }
        // Strange bug that can only be resolved by repeating identical IF statment.
        // Sharing violation error 
        if (entry.path().string().find(".json") != std::string::npos)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            //std::filesystem::remove(entry.path().string());
            publish_command = "del " + entry.path().string().substr(entry.path().string().find_last_of("/\\") + 1);
            std::cout << "[-] Deleting: " + publish_command << "\n";
            system(publish_command.c_str());
            std::cout << "\n";
        }
    }
}

void kafka_direct_producer(nlohmann::json json_record)
{
    using namespace kafka;
    using namespace kafka::clients::producer;

    std::cout << "producer()" << "\n";

    // Environment Variable = KAFKA_BROKER_LIST

    const std::string broker_ip = "192.168.1.119:9092";

    Topic topic = "Placeholder";

    std::cout << "JSON_Record_Dump" << "\n";
    std::cout << json_record.dump() << "\n";
    std::cout << json_record.at("emotion") << '\n';

    if (json_record.at("emotion") == "Happy")
    {
        std::cout << "Key: Happy" << "\n";
        topic = "emotion_happy";
    }
    else if (json_record.at("emotion") == "Sad")
    {
        std::cout << "Key: Sad" << "\n";
        topic = "emotion_sad";
    }
    else
    {
        std::cout << "Not an emotion of interest" << "\n";
        topic = "emotion_irrelevant";
    }

    const Properties props({ {"bootstrap.servers", broker_ip} });

    KafkaProducer producer(props);

    // Convert JSON to record for producer.
    ProducerRecord record(topic, NullKey, Value(json_record.dump().c_str(), json_record.dump().size()));
    
    // Kafka listeners configuration
    // server.properties
    // listeners=PLAINTEXT://192.168.1.119:9092

    // Connectivity Testing:
    // telnet 192.168.1.119 9092
    // .\ncat.exe -v 192.168.1.119 -p 9092
    // nc -vz 192.168.1.119 9092

    // Enable Connectivity:
    // 1) Configure firewall on both guest and host to allow communication.
    // 2) Configure Kafka broker's server.properties file to listen

    auto deliveryCb = [](const RecordMetadata& metadata, const Error& error) 
    {
        if (!error) {
            std::cout << "Message delivered: " << metadata.toString() << std::endl;
        }
        else {
            std::cerr << "Message failed to be delivered: " << error.message() << std::endl;
        }
    };
    producer.send(record, deliveryCb);
    producer.close();
}

void json_debug(nlohmann::json json_record)
{
    std::cout << "JSON_Record_Dump" << "\n";
    std::cout << json_record.dump() << "\n";
    std::cout << json_record.at("emotion") << '\n';
    std::cout << json_record.dump().c_str() << "\n";
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
    json_debug(emotion_event_creation());

    /**
    for (int i = 0; i <= 10; i++)
    {
        kafka_direct_producer(emotion_event_creation());
    }
    **/
    std::cout << "[!] END" << "\n";
    std::cout << "[!] Exiting..." << "\n\n";
    system("pause");
}

/**

Kafka_Emotion_Garbage_Producer -> Kafka Instance -> Consumer

Sample Json
{
    "name": "John Doe",
    "emotion": "Happy"
    "physical state": "Idle"
    "source": "Device02"
}

// Create topics for Kafka broker.
$ bin/kafka-topics.sh --create --topic emotion_happy --bootstrap-server localhost:9092
$ bin/kafka-topics.sh --create --topic emotion_sad --bootstrap-server localhost:9092

**/
