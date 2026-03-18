#include <iostream>
#include <string>
#include <cctype>

#include "dashboard.h"

bool isNumber(const std::string& str) {
    for(char c:str) if(!std::isdigit(c)) return false;
    return !str.empty();
}

int main(int argc, char* argv[]) {

    if(argc < 2){
        runDashboard();
        return 0;
    }

    std::string command = argv[1];

    if(isNumber(command)){
        showSessionDetail(std::stoi(command));
        return 0;
    }

    if(command == "history"){
        showFullHistory();
        return 0;
    }

    if(command == "daemon"){
        runDaemon();
        return 0;
    }

    if(command == "help"){
        showHelp();
        return 0;
    }

    if(command == "about"){
        showAbout();
        return 0;
    }

    std::cout<<"Unknown command\n";
    std::cout<<"Try: uptimer help\n";

    return 0;
}
