// server.cpp
#include "common.h"

SOCKET serverSocket;
std::map<SOCKET, std::string> clients;
std::mutex mtx;

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

        if (msg == "/exit\n") {
            break;
        }

        // Handle commands
        if (msg.rfind("/list", 0) == 0) {
            std::string userList = "Online users:\n";
            std::lock_guard<std::mutex> lock(mtx);
            for (auto& user : clients)
                userList += user.second + "\n";
            send(clientSocket, userList.c_str(), userList.length(), 0);
            continue;
        }

        // Forward everything (text or file data)
        if (msg.rfind("FILE:", 0) == 0 || msg == "FILE_END\n") {
            broadcast(buffer, bytesReceived, clientSocket);
        } else if (msg[0] == 0x00 || msg[0] == 0xFF) {
            // binary chunks, forward as-is
            broadcast(buffer, bytesReceived, clientSocket);
        } else {
            std::string fullMsg = clients[clientSocket] + ": " + msg;
            broadcast(fullMsg.c_str(), fullMsg.length(), clientSocket);
        }
    }

    std::cout << "Client disconnected.\n";
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

    std::cout << "Server listening on port " << PORT << "\n";

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
