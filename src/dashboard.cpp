#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>
#include <termios.h>

#include "uptime.h"
#include "dashboard.h"

#define HISTORY_FILE "/var/lib/uptimer/history.log"

struct Session {
    long bootTime;
    int id;
    std::string startTime;
    std::string endTime;
    std::string uptime;
    std::string date;
};

// ===== GLOBALS =====
bool running = true;
struct termios oldt;

// ===== SIGNAL HANDLER =====
void signalHandler(int sig){
    running = false;
}

// ===== TERMINAL CONTROL =====
void disableCtrlCDisplay(){
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~ECHOCTL; // hide ^C
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restoreTerminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// ===== UTIL =====
void ensureDataDir() {
    mkdir("/var/lib/uptimer", 0755);
}

std::string formatTime(long seconds) {
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;

    std::ostringstream oss;
    oss << h << "h " << m << "m " << s << "s";
    return oss.str();
}

std::string formatTimeAMPM(std::tm* tmPtr) {
    int hour = tmPtr->tm_hour;
    std::string ampm = (hour >= 12) ? "PM" : "AM";

    hour %= 12;
    if(hour==0) hour=12;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ":"
        << std::setw(2) << std::setfill('0') << tmPtr->tm_min << ":"
        << std::setw(2) << std::setfill('0') << tmPtr->tm_sec
        << " " << ampm;

    return oss.str();
}

std::string currentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* tmPtr = std::localtime(&now);

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << tmPtr->tm_mday
        << " " << std::put_time(tmPtr, "%b %Y");

    return oss.str();
}

std::time_t getSystemBootTime() {
    std::ifstream uptimeFile("/proc/uptime");
    double uptimeSeconds;
    uptimeFile >> uptimeSeconds;

    std::time_t now = std::time(nullptr);
    return now - (long)uptimeSeconds;
}

// ===== FILE HANDLING =====
std::vector<Session> readHistory() {
    std::vector<Session> sessions;
    std::ifstream file(HISTORY_FILE);

    if(!file.is_open()) return sessions;

    std::string line;

    while(std::getline(file,line)) {

        if(line.empty()) continue;

        Session s;
        std::istringstream iss(line);

        iss >> s.bootTime;
        iss.ignore(3);
        iss >> s.id;

        size_t pos,end;

        pos=line.find("Start Time: ");
        end=line.find(" | End Time:");
        s.startTime = line.substr(pos+12,end-(pos+12));

        pos=line.find("End Time: ");
        end=line.find(" | Uptime:");
        s.endTime = line.substr(pos+10,end-(pos+10));

        pos=line.find("Uptime: ");
        end=line.find(" | Date:");
        s.uptime = line.substr(pos+8,end-(pos+8));

        pos=line.find("Date: ");
        s.date = line.substr(pos+6);

        sessions.push_back(s);
    }

    return sessions;
}

void writeHistory(const std::vector<Session>& sessions) {
    std::ofstream file(HISTORY_FILE,std::ios::trunc);

    for(const auto& s:sessions){
        file << s.bootTime
             << " | " << s.id
             << " | Start Time: " << s.startTime
             << " | End Time: " << s.endTime
             << " | Uptime: " << s.uptime
             << " | Date: " << s.date
             << "\n";
    }
}

int getNextID(const std::vector<Session>& sessions) {
    int maxID = 0;

    for(const auto& s:sessions)
        if(s.id>maxID)
            maxID=s.id;

    return maxID+1;
}

// ===== UI =====
void displayLast3(const std::vector<Session>& sessions) {

    std::cout<<"Recent Sessions\n\n";

    std::cout<<"┌──────┬──────────────┬──────────────┐\n";
    std::cout<<"│ ID   │ Uptime       │ Date         │\n";
    std::cout<<"├──────┼──────────────┼──────────────┤\n";

    int count=0;

    for(int i=sessions.size()-1;i>=0 && count<3;i--) {

        std::cout<<"│ "<<std::left<<std::setw(4)<<"#"+std::to_string(sessions[i].id)
                 <<" │ "<<std::setw(12)<<sessions[i].uptime.substr(0,sessions[i].uptime.find("m")+1)
                 <<" │ "<<std::setw(12)<<sessions[i].date
                 <<" │\n";

        count++;
    }

    if(sessions.empty())
        std::cout<<"│ --   │ No records   │ --           │\n";

    std::cout<<"└──────┴──────────────┴──────────────┘\n";
}

// ===== MAIN DASHBOARD =====
void runDashboard() {

    ensureDataDir();

    disableCtrlCDisplay();
    signal(SIGINT, signalHandler);

    auto sessions = readHistory();

    std::cout<<"══════════════ UPTIMER ══════════════\n\n";

    displayLast3(sessions);

    std::cout<<"\n═════════════════════════════════════\n\n";

    std::cout<<"Live Session\n\n";

    long bootTime = getSystemBootTime();

    while(running) {

        long uptime = getSystemUptime();

        bool found=false;

        for(auto &s:sessions){
            if(s.bootTime==bootTime){
                s.uptime=formatTime(uptime);

                std::time_t now = std::time(nullptr);
                s.endTime = formatTimeAMPM(std::localtime(&now));

                found=true;
                break;
            }
        }

        if(!found){
            Session s;

            s.bootTime=bootTime;
            s.id=getNextID(sessions);

            s.startTime = formatTimeAMPM(std::localtime(&bootTime));

            std::time_t now = std::time(nullptr);
            s.endTime = formatTimeAMPM(std::localtime(&now));

            s.uptime = formatTime(uptime);
            s.date = currentDate();

            sessions.push_back(s);
        }

        writeHistory(sessions);

        std::cout<<"\rUptime ─⮞ "<<std::setw(15)<<formatTime(uptime)<<"        "<<std::flush;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // graceful exit UI
    std::cout<<"\n═════════════════════════════════════\n";

    restoreTerminal();
}

// ===== EXTRA COMMANDS =====
void showSessionDetail(int id) {

    auto sessions = readHistory();

    for(auto &s:sessions){
        if(s.id==id){

            std::cout<<"════════════════ SESSION DETAIL ════════════════\n";
            std::cout<<"Session ID ─⮞ "<<s.id<<"\n\n";
            std::cout<<"Start Time ─⮞ "<<s.startTime<<"\n";
            std::cout<<"End Time   ─⮞ "<<s.endTime<<"\n";
            std::cout<<"Uptime     ─⮞ "<<s.uptime<<"\n";
            std::cout<<"Date       ─⮞ "<<s.date<<"\n";
            std::cout<<"══════════════════════════════════════════════\n";

            return;
        }
    }

    std::cout<<"Session not found\n";
}

void showFullHistory() {

    auto sessions = readHistory();

    for(int i=sessions.size()-1;i>=0;i--){

        auto &s=sessions[i];

        std::cout<<"════════════════ SESSION DETAIL ════════════════\n";
        std::cout<<"Session ID ─⮞ "<<s.id<<"\n\n";
        std::cout<<"Start Time ─⮞ "<<s.startTime<<"\n";
        std::cout<<"End Time   ─⮞ "<<s.endTime<<"\n";
        std::cout<<"Uptime     ─⮞ "<<s.uptime<<"\n";
        std::cout<<"Date       ─⮞ "<<s.date<<"\n";
        std::cout<<"══════════════════════════════════════════════\n\n";
    }
}

void runDaemon() {

    ensureDataDir();

    long bootTime = getSystemBootTime();
    auto sessions = readHistory();

    int index=-1;

    for(size_t i=0;i<sessions.size();i++){
        if(sessions[i].bootTime==bootTime){
            index=i;
            break;
        }
    }

    if(index==-1){
        Session s;

        s.bootTime=bootTime;
        s.id=getNextID(sessions);

        s.startTime = formatTimeAMPM(std::localtime(&bootTime));

        std::time_t now = std::time(nullptr);
        s.endTime = formatTimeAMPM(std::localtime(&now));

        s.uptime = "0h 0m 0s";
        s.date = currentDate();

        sessions.push_back(s);
        index = sessions.size()-1;
    }

    while(true){

        std::time_t now = std::time(nullptr);

        sessions[index].endTime = formatTimeAMPM(std::localtime(&now));
        sessions[index].uptime = formatTime(now - bootTime);

        writeHistory(sessions);

        sleep(5);
    }
}

// ===== HELP MENU =====
void showHelp() {

    std::cout<<"══════════════ UPTIMER HELP ══════════════\n\n";

    std::cout<<"Usage:\n\n";

    std::cout<<"  uptimer                ─⮞ Launch live dashboard\n";
    std::cout<<"  uptimer history        ─⮞ Show full session history\n";
    std::cout<<"  uptimer <id>           ─⮞ Show specific session detail\n";
    std::cout<<"  uptimer help           ─⮞ Show this help menu\n";
    std::cout<<"  uptimer about          ─⮞ About this tool\n";

    std::cout<<"\n═══════════════════════════════════════════\n";
}


// ===== ABOUT =====
void showAbout() {

    std::cout<<"══════════════ ABOUT UPTIMER ══════════════\n\n";

    std::cout<<"Uptimer ─⮞ Linux Uptime Tracker & Logger\n\n";

    std::cout<<"A lightweight system tool that tracks system\n";
    std::cout<<"uptime sessions, stores history, and provides\n";
    std::cout<<"a clean CLI dashboard experience.\n\n";

    std::cout<<"Features:\n";
    std::cout<<"• Live uptime dashboard\n";
    std::cout<<"• Persistent session history\n";
    std::cout<<"• Systemd background service\n";
    std::cout<<"• Session-based tracking with IDs\n\n";

    std::cout<<"Author ─⮞ Jibran\n";
    std::cout<<"Built with ❤ on Linux\n";
    std::cout<<"Visit : www.Jibranzada.com \n";

    std::cout<<"\n═══════════════════════════════════════════\n";
}




