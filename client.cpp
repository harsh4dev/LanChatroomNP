#include "common.h"
#include <sys/stat.h>  // for mkdir
#include <sys/types.h>
#ifdef _WIN32
  #include <direct.h>  // for _mkdir
#endif

SOCKET clientSocket;
std::string myName;

std::string currentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[10];
    snprintf(buf, sizeof(buf), "[%02d:%02d]", ltm->tm_hour, ltm->tm_min);
    return std::string(buf);
}

void logToFile(const std::string& text) {
    std::ofstream log("chatlog.txt", std::ios::app);
    log << currentTime() << " " << text;
    log.close();
}

void receiveMessages() {
    char buffer[BUFFER_SIZE];
    bool receivingFile = false;
    std::ofstream outFile;
    std::string filePath;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer, bytesReceived);

        // File handling
        if (msg.rfind("FILE:", 0) == 0) {
            std::string filename = msg.substr(5);
            filename.erase(filename.find_last_not_of(" \n\r\t") + 1);

            // Create received folder if it doesn't exist
#ifdef _WIN32
            _mkdir("received");
#else
            mkdir("received", 0777);
#endif

            filePath = "received/" + filename;
            outFile.open(filePath, std::ios::binary);
            receivingFile = true;

            std::cout << currentTime() << " Receiving file: " << filename << "\n";
            continue;
        }

        if (msg == "FILE_END\n") {
            receivingFile = false;
            if (outFile.is_open()) {
                outFile.close();
                std::cout << currentTime() << " File received and saved to " << filePath << "\n";
            }
            continue;
        }

        if (receivingFile && outFile.is_open()) {
            outFile.write(buffer, bytesReceived);
        } else {
            std::string display;
            if (msg.rfind(myName + ": ", 0) == 0) {
                std::string body = msg.substr(myName.length() + 2);
                display = currentTime() + " You: " + body;
            } else {
                display = currentTime() + " " + msg;
            }
            std::cout << display;
            logToFile(display);
        }
    }
}

void sendFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "File not found: " << filename << "\n";
        return;
    }

    std::string header = "FILE:" + filename + "\n";
    send(clientSocket, header.c_str(), header.length(), 0);

    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        int bytes = file.gcount();
        send(clientSocket, buffer, bytes, 0);
    }

    std::string end = "FILE_END\n";
    send(clientSocket, end.c_str(), end.length(), 0);
    std::cout << "File sent successfully!\n";
    file.close();
}

int main() {
#ifdef _WIN32
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
#endif

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverHint = {};
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(PORT);
#ifdef _WIN32
    serverHint.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
    inet_pton(AF_INET, "127.0.0.1", &serverHint.sin_addr);
#endif

    connect(clientSocket, (sockaddr*)&serverHint, sizeof(serverHint));

    char buffer[BUFFER_SIZE];
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    std::cout << buffer;

    std::getline(std::cin, myName);
    send(clientSocket, myName.c_str(), myName.size(), 0);

    std::thread t(receiveMessages);
    t.detach();

    while (true) {
        std::string input;
        std::getline(std::cin, input);
        input += "\n";

        if (input.rfind("/file ", 0) == 0) {
            std::string filename = input.substr(6);
            filename.erase(filename.find_last_not_of(" \n\r\t") + 1);
            sendFile(filename);
        } else if (input == "/help\n") {
            std::cout <<
                "\nAvailable Commands:\n"
                "  /file <filename> → send file\n"
                "  /list            → show online users\n"
                "  /exit            → leave the chat\n"
                "  /help            → show this help\n\n";
        } else {
            send(clientSocket, input.c_str(), input.size(), 0);
            if (input == "/exit\n") break;
        }
    }

    CLOSESOCKET(clientSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
