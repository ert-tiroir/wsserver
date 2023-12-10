
/**********************************************************************************/
/* wsserver/server.cpp                                                            */
/*                                                                                */
/* This file contains the implemenation of the WebSocketServer structure          */
/**********************************************************************************/
/*                  This file is part of the ERT-Tiroir project                   */
/*                         github.com/ert-tiroir/wsserver                         */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 ert-tiroir                                                  */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#include <wsserver/server.h>
#include <wsserver/http.h>

#include <wsserver/sha1/sha1.h>
#include <iostream>

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fcntl.h>

using namespace std;

const auto __s_listen = listen;
const auto __s_close  = close;

const string PADDING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void WebSocketServer::init (int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return ;

    cout << "Created socket\n";

    int opt = 1;

    if (setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cout << "Could not set options\n";
        return ;
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(port);

    if (bind(sockfd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        cout << "Could not bind address\n";
        return ;
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
}
bool WebSocketServer::listen () {
    int res = __s_listen(sockfd, 3);
    if (__s_listen(sockfd, 3) < 0) {
        cout << "Could not listen to socket\n";
        return false;
    }

    int new_socket = accept(sockfd,(struct sockaddr*)&address,
                  &addrlen);
    if (new_socket < 0) return false;

    cout << "Socket : " << new_socket << endl;

    char buffer[1024] = { 0 };

    ssize_t valread = read(new_socket, buffer, 1024 - 1);
    while (valread <= 0)
        valread = read(new_socket, buffer, 1024 - 1);

    HttpRequest request = HttpRequest(string(buffer));

    if (!request.isWebSocketRequest()) {
        cout << "Non web socket request detected\n";
        __s_close(new_socket);
        return false;
    }

    HttpResponse response = HttpResponse(request, 101, "Switching protocols");
    response.setHeader("Upgrade",    "websocket");
    response.setHeader("Connection", "Upgrade");

    string keyBuffer = request.getHeader("Sec-WebSocket-Key") + PADDING;
    
    SHA1 hasher;
    hasher.update( keyBuffer );
    string result = hasher.final_base64();

    response.setHeader("Sec-WebSocket-Accept", result);
    response.setHeader("Sec-WebSocket-Protocol", "chat");

    string respBuffer = response.as_string();
    const char* respData = respBuffer.c_str();

    int sent = send(new_socket, respData, strlen(respData), 0);

    if (sent == -1) {
        __s_close(new_socket);
        return false;
    }
    WebSocketClient* client = new WebSocketClient();
    client->init(new_socket);
    clients.push_back(client);
    fcntl(new_socket, F_SETFL, O_NONBLOCK);
    return true;
}
void WebSocketServer::close (string message) {
    for (WebSocketClient* client : clients) client->close(message);

    __s_close(sockfd);
}

void WebSocketServer::broadcast (string &message) {
    for (WebSocketClient* client : clients) client->send(message);
}
void WebSocketServer::receive (ReceiveCallback callback) {
    for (WebSocketClient* client : clients) {
        optional<string> result = client->recv();

        if (result.has_value())
            callback(client, result.value());
    }
}

int WebSocketServer::clientCount () { return clients.size(); }
WebSocketClient* WebSocketServer::getClient (int id) {
    return clients[id];
}
