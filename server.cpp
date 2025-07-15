// server.cpp
#include "common.h"

SOCKET serverSocket;
std::map<SOCKET, std::string> clients;
std::mutex mtx;

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

void broadcast(const char* data, int length, SOCKET sender) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& client : clients) {
        if (client.first != sender) {
            send(client.first, data, length, 0);
        }
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer, bytesReceived);

        if (msg == "/exit\n") break;

        if (msg.rfind("/list", 0) == 0) {
            std::string userList = "Online users:\n";
            std::lock_guard<std::mutex> lock(mtx);
            for (auto& user : clients)
                userList += user.second + "\n";
            send(clientSocket, userList.c_str(), userList.length(), 0);
            continue;
        }

        if (msg.rfind("FILE:", 0) == 0 || msg == "FILE_END\n") {
            setColor(14); // Yellow
            std::cout << currentTime() << " [File Transfer] " << clients[clientSocket] << ": " << msg;
            setColor(7);
            broadcast(buffer, bytesReceived, clientSocket);
        } else {
            std::string fullMsg = clients[clientSocket] + ": " + msg;
            setColor(15); // White
            std::cout << currentTime() << " " << fullMsg;
            setColor(7);
            broadcast(fullMsg.c_str(), fullMsg.length(), clientSocket);
        }
    }

    std::cout << "[Server] Client disconnected.\n";
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(clientSocket);
    closesocket(clientSocket);
}

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverHint = {};
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(PORT);
    serverHint.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverHint, sizeof(serverHint));
    listen(serverSocket, MAX_CLIENTS);

    std::cout << "Server started on port " << PORT << "\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        send(clientSocket, "Enter your username: ", 22, 0);

        char nameBuffer[BUFFER_SIZE];
        recv(clientSocket, nameBuffer, BUFFER_SIZE, 0);

        std::lock_guard<std::mutex> lock(mtx);
        clients[clientSocket] = std::string(nameBuffer);

        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
