// client.cpp
#include "common.h"

SOCKET clientSocket;

void receiveMessages() {
    char buffer[BUFFER_SIZE];
    bool receivingFile = false;
    std::ofstream outFile;
    
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer, bytesReceived);

        // Check for file header
        if (msg.rfind("FILE:", 0) == 0) {
            std::string filename = msg.substr(5);
            filename.erase(filename.find_last_not_of(" \n\r\t") + 1);
            outFile.open(filename, std::ios::binary);
            receivingFile = true;
            std::cout << "Receiving file: " << filename << "\n";
            continue;
        }

        // Check for end of file
        if (msg == "FILE_END\n") {
            receivingFile = false;
            if (outFile.is_open()) {
                outFile.close();
                std::cout << "File received successfully!\n";
            }
            continue;
        }

        // Write file data
        if (receivingFile && outFile.is_open()) {
            outFile.write(buffer, bytesReceived);
        } else {
            std::cout << msg;
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

    std::string name;
    std::getline(std::cin, name);
    send(clientSocket, name.c_str(), name.size(), 0);

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
