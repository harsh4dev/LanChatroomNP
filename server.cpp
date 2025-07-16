#include "common.h"

SOCKET serverSocket;
std::map<SOCKET, std::string> clients;
std::mutex mtx;

std::string currentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[10];
    snprintf(buf, sizeof(buf), "[%02d:%02d]", ltm->tm_hour, ltm->tm_min);
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

    // Get the username first
    char nameBuffer[BUFFER_SIZE];
    recv(clientSocket, nameBuffer, BUFFER_SIZE, 0);

    {
        std::lock_guard<std::mutex> lock(mtx);
        clients[clientSocket] = std::string(nameBuffer);
    }

    std::cout << currentTime() << " User connected: " << clients[clientSocket] << "\n";

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer, bytesReceived);

        if (msg == "/exit\n") break;

        if (msg.rfind("/list", 0) == 0) {
            std::string list = "Online users:\n";
            std::lock_guard<std::mutex> lock(mtx);
            for (auto& user : clients)
                list += user.second + "\n";
            send(clientSocket, list.c_str(), list.length(), 0);
            continue;
        }

        if (msg.rfind("FILE:", 0) == 0 || msg == "FILE_END\n") {
            std::cout << currentTime() << " [File] " << clients[clientSocket] << ": " << msg;
            broadcast(buffer, bytesReceived, clientSocket);
        } else {
            std::string full = clients[clientSocket] + ": " + msg;
            std::cout << currentTime() << " " << full;
            broadcast(full.c_str(), full.length(), clientSocket);
        }
    }

    // Log disconnection
    std::cout << currentTime() << " User disconnected: " << clients[clientSocket] << "\n";

    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(clientSocket);
    CLOSESOCKET(clientSocket);
}

int main() {
#ifdef _WIN32
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
#endif

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
        std::thread t([clientSocket]() { handleClient(clientSocket); });
        t.detach();
    }

    CLOSESOCKET(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
