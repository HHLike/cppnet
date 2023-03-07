#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <sys/select.h>

using namespace std;

const int PORT = 8888; // 服务器端口
const int MAX_CLIENTS = 10; // 最大客户端数量
const int BUFFER_SIZE = 1024; // 缓冲区大小

#endif