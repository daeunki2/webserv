/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:14:04 by daeunki2          #+#    #+#             */
/*   Updated: 2025/10/19 15:47:27 by daeunki2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdio>

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
   const int PORT = 8080;

    // 1. 소켓 생성
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
	{
        perror("socket failed");
        return 1;
    }

    // 2. 주소 바인딩
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
        perror("bind failed");
        return 1;
    }

    // 3. 리슨
    if (listen(server_fd, 3) < 0)
	{
        perror("listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // 4. 클라이언트 연결 처리
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_fd < 0)
	{
        perror("accept failed");
        return 1;
    }
    std::cout << "Client connected!" << std::endl;


    // 5. Request 수신
    char buffer[4096] = {0};
    ssize_t valread = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (valread < 0) { perror("recv failed"); }
    else {
        buffer[valread] = '\0'; // 문자열 종료
        std::cout << "----- Request Received -----\n";
        std::cout << buffer << std::endl;
        std::cout << "----------------------------\n";
    }

    // 5. 최소 HTTP 응답
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 100\r\n"
        "\r\n"
        "<h1>Hello World~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!</h1>";

    send(client_fd, response, strlen(response), 0);
    std::cout << "Response sent!" << std::endl;

    // 6. 소켓 종료
    close(client_fd);
    close(server_fd);

    return 0;
}


//    http://127.0.0.1:8080

/*
how to get a msg from client.
1. open 2 terminal
2. run server in terminal a
3. connect to a server and send a request by run this cmd in terminal b
for GET "curl -v http://127.0.0.1:8080/index.html"
for POST "curl -v -X POST -d "username=manoi&age=29" http://127.0.0.1:8080/upload"
for HEAD "curl -v -I http://127.0.0.1:8080/"
*/