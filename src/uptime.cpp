#include <fstream>
#include "uptime.h"

long getSystemUptime() {
    std::ifstream file("/proc/uptime");

    double uptimeSeconds = 0;

    if (file.is_open()) {
        file >> uptimeSeconds;
        file.close();
    }

    return (long)uptimeSeconds;
}
