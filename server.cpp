// server.cpp
#include "common.h"

SOCKET serverSocket;
std::map<SOCKET, std::string> clients;
std::mutex mtx;

void broadcast(const std::string& msg, SOCKET sender) {
    mtx.lock();
    for (auto& client : clients) {
        if (client.first != sender) {
            send(client.first, msg.c_str(), msg.length(), 0);
        }
    }
    mtx.unlock();
}

void sendFile(SOCKET client, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::string errorMsg = "Server: File not found.\n";
        send(client, errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    std::string header = "FILE_TRANSFER " + filename + "\n";
    send(client, header.c_str(), header.size(), 0);

    char buffer[BUFFER_SIZE];
    while (!file.eof()) {
        file.read(buffer, BUFFER_SIZE);
        send(client, buffer, file.gcount(), 0);
    }

    std::string endMsg = "FILE_END\n";
    send(client, endMsg.c_str(), endMsg.size(), 0);
    file.close();
}

void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected.\n";
            closesocket(clientSocket);
            mtx.lock();
            clients.erase(clientSocket);
            mtx.unlock();
            break;
        }

        std::string msg(buffer);
        if (msg.rfind("/file ", 0) == 0) {
            std::string filename = msg.substr(6);
            sendFile(clientSocket, filename);
        } else if (msg == "/list\n") {
            std::string userList = "Online Users:\n";
            mtx.lock();
            for (auto& user : clients) {
                userList += user.second + "\n";
            }
            mtx.unlock();
            send(clientSocket, userList.c_str(), userList.size(), 0);
        } else if (msg == "/exit\n") {
            std::cout << "Client requested exit.\n";
            closesocket(clientSocket);
            mtx.lock();
            clients.erase(clientSocket);
            mtx.unlock();
            break;
        } else {
            std::string fullMsg = clients[clientSocket] + ": " + msg;
            std::cout << fullMsg;
            broadcast(fullMsg, clientSocket);
        }
    }
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
        std::string username(nameBuffer);

        mtx.lock();
        clients[clientSocket] = username;
        mtx.unlock();

        std::thread t(handleClient, clientSocket);
        t.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
