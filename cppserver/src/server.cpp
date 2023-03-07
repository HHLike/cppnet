#include "server.hpp"

int main() {
    // 创建服务器端套接字
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "socket failed" << endl;
        return 1;
    }
    // 设置套接字为非阻塞模式
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    // 设置服务器端地址
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    // 绑定套接字和地址
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1) {
        cerr << "bind failed" << endl;
        return 1;
    }
    // 监听套接字
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        cerr << "listen failed" << endl;
        return 1;
    }
    cout << "server is listening on port " << PORT << endl;
    // 创建客户端套接字数组
    vector<int> client_sockets;
    // 创建文件描述符集合
    fd_set readfds;
    // 创建缓冲区
    char buffer[BUFFER_SIZE];
    int bytes_received; // 接收到的字节数
    // 服务器主循环
    while (true) {
        // 清空文件描述符集合
        FD_ZERO(&readfds);
        // 将服务器端套接字加入集合
        FD_SET(server_socket, &readfds);
        // 将客户端套接字加入集合
        for (int i = 0; i < client_sockets.size(); i++) {
            FD_SET(client_sockets[i], &readfds);
        }
        // 使用select函数监视文件描述符集合
        int max_fd = server_socket; // 最大的文件描述符
        if (!client_sockets.empty()) {
            max_fd = max(server_socket, client_sockets.back()); // 如果有客户端连接，取最后一个客户端套接字和服务器端套接字的较大者
        }
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL); // 阻塞等待文件描述符集合中的套接字有活动
        if (activity == -1) {
            cerr << "select failed" << endl;
            break;
        }
        // 检查服务器端套接字是否有活动
        if (FD_ISSET(server_socket, &readfds)) {
            // 接受新的客户端连接
            sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);
            int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_len);
            if (client_socket == -1) {
                cerr << "accept failed" << endl;
                break;
            }
            // 设置客户端套接字为非阻塞模式
            fcntl(client_socket, F_SETFL, O_NONBLOCK);
            // 将客户端套接字加入数组
            client_sockets.push_back(client_socket);
            // 显示客户端信息
            cout << "new client connected: " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;
        }
        // 检查客户端套接字是否有活动
        for (int i = 0; i < client_sockets.size(); i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {
                // 接收客户端发送的数据
                bytes_received = recv(client_sockets[i], buffer, BUFFER_SIZE, 0);
                if (bytes_received == -1) {
                    cerr << "recv failed" << endl;
                    break;
                }
                if (bytes_received == 0) {
                    // 客户端断开连接
                    cout << "client disconnected" << endl;
                    // 关闭客户端套接字
                    close(client_sockets[i]);
                    // 从数组中移除客户端套接字
                    client_sockets.erase(client_sockets.begin() + i);
                    i--; // 调整索引
                    continue;
                }
                // 在控制台显示接收到的数据
                buffer[bytes_received] = '\0';
                cout << "received: " << buffer << endl;
                // 将接收到的数据原样发送回客户端
                if (send(client_sockets[i], buffer, bytes_received, 0) == -1) {
                    cerr << "send failed" << endl;
                    break;
                }
            }
        }
    }
    // 关闭服务器端套接字
    close(server_socket);
    return 0;
}