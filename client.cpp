// client.cpp
#include "common.h"

SOCKET clientSocket;
std::string myName;

void setColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

std::string currentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[10];
    sprintf(buf, "[%02d:%02d]", ltm->tm_hour, ltm->tm_min);
    return std::string(buf);
}

void receiveMessages() {
    char buffer[BUFFER_SIZE];
    bool receivingFile = false;
    std::ofstream outFile;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer, bytesReceived);

        // Handle file reception
        if (msg.rfind("FILE:", 0) == 0) {
            std::string filename = msg.substr(5);
            filename.erase(filename.find_last_not_of(" \n\r\t") + 1);
            outFile.open(filename, std::ios::binary);
            receivingFile = true;
            setColor(14); // Yellow
            std::cout << currentTime() << " Receiving file: " << filename << "\n";
            setColor(7);
            continue;
        }

        if (msg == "FILE_END\n") {
            receivingFile = false;
            if (outFile.is_open()) {
                outFile.close();
                setColor(10); // Green
                std::cout << currentTime() << " File received successfully!\n";
                setColor(7);
            }
            continue;
        }

        if (receivingFile && outFile.is_open()) {
            outFile.write(buffer, bytesReceived);
        } else {
            // Detect if this message is from self
            if (msg.rfind(myName + ": ", 0) == 0) {
                std::string sentText = msg.substr(myName.length() + 2);
                setColor(11); // Cyan
                std::cout << currentTime() << " You: " << sentText;
            } else {
                setColor(15); // White
                std::cout << currentTime() << " " << msg;
            }
            setColor(7);
        }
    }
}

void sendFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        setColor(12);
        std::cout << "File not found: " << filename << "\n";
        setColor(7);
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

    std::string endMsg = "FILE_END\n";
    send(clientSocket, endMsg.c_str(), endMsg.length(), 0);
    std::cout << "File sent successfully!\n";
    file.close();
}

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverHint = {};
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(PORT);
    serverHint.sin_addr.s_addr = inet_addr("127.0.0.1");
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
        } else {
            send(clientSocket, input.c_str(), input.size(), 0);
            if (input == "/exit\n") break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
