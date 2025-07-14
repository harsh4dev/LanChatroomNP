// common.h
#ifndef COMMON_H
#define COMMON_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <map>
#include <vector>
#include <fstream>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

#define PORT 54000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

#endif
