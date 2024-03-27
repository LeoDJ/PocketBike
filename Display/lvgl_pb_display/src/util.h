#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iomanip>

extern std::string curIpStr, curSsidStr;
extern std::mutex mutexIpSsid;
extern size_t curMqttQueued;

class Util {
  public:
    static std::string getCurrentSSID() {
        std::string ssid;
        std::string command = "/usr/sbin/iwgetid -r"; // Command to retrieve SSID

        // Open a pipe to execute the command and read the output
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Error executing command." << std::endl;
            return ssid;
        }

        // Read the output from the command
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            ssid += buffer;
        }

        // Close the pipe
        pclose(pipe);

        // Remove the newline character at the end of the SSID string
        ssid.erase(ssid.find_last_not_of("\n") + 1);

        return ssid;
    }

    static std::string getIPAddress_old() {
        int sockfd;
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Failed to create socket." << std::endl;
            return "";
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Failed to connect to a remote host." << std::endl;
            close(sockfd);
            return "";
        }

        getsockname(sockfd, (struct sockaddr*)&addr, &addrLen);
        close(sockfd);

        return std::string(inet_ntoa(addr.sin_addr));
    }

    static std::string getIPAddress() {
        std::string ipAddress;
        FILE* pipe = popen("hostname -I", "r");
        if (!pipe) {
            std::cerr << "Failed to execute command." << std::endl;
            return "";
        }

        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            ipAddress = buffer;
        }

        pclose(pipe);

        // Remove trailing newline character
        if (!ipAddress.empty() && ipAddress[ipAddress.length() - 1] == '\n') {
            ipAddress.erase(ipAddress.length() - 1);
        }

        return ipAddress;
    }

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(value % std::chrono::seconds(1));

        // Format time
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << millis.count();
        return ss.str();
    }
};
