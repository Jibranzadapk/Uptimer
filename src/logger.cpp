#include <iostream>
#include <fstream>
#include <ctime>
#include "logger.h"

#define HISTORY_FILE "data/history.log"

void saveUptime(long seconds) {
    std::ofstream file(HISTORY_FILE, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error opening history file\n";
        return;
    }

    std::time_t now = std::time(nullptr);

    file << now << " | " << seconds << std::endl;

    file.close();
}

void showHistory() {
    std::ifstream file(HISTORY_FILE);

    if (!file.is_open()) {
        std::cerr << "No history found.\n";
        return;
    }

    long timestamp;
    long seconds;
    char separator;

    while (file >> timestamp >> separator >> seconds) {

        std::time_t t = timestamp;
        std::tm* timeinfo = std::localtime(&t);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;

        std::cout << buffer << " | "
                  << hours << "h "
                  << minutes << "m "
                  << secs << "s\n";
    }

    file.close();
}
