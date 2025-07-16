#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <vector>
#include <fstream>
#include <mutex>
#include <cstring>
#include <ctime>

#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #define CLOSESOCKET closesocket
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #define SOCKET int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define CLOSESOCKET close
#endif

#define PORT 54000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

#endif
