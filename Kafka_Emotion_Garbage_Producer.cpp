#include <iostream>
#include <vector>

int main()
{

    std::vector<std::string> primary_emotions = {"\033[93mHappy", "\033[36mSad", "\033[91mAnger", "\033[95mFear", "\033[92mDisgust", "\033[96mSurprise"};
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

**/
