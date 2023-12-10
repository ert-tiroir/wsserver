
/**********************************************************************************/
/* wsserver/client.h                                                              */
/*                                                                                */
/* This file contains the implementations of the web socket client and in         */
/* particular implements the details of the WebSocket protocol                    */
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

#include <wsserver/client.h>
#include <unistd.h>
#include <sys/socket.h>

const auto __s_send  = send;
const auto __s_close = close;

void WebSocketClient::init (int sockfd) {
    if (closed) return ;

    this->sockfd = sockfd;
    this->closed = false;
}

char buffer[1024];

optional<string> WebSocketClient::recv() {
    if (closed) return {};

    ssize_t valread = read(sockfd, buffer, 2);
    if (valread != 2) return {};
        
    unsigned int c0 = buffer[0];

    bool finish = (c0 & 128) != 0;
    int op_code = (c0 & 15);
    int rsv     = (c0 >>  4) % 8;
    if (op_code == 8) {
        this->closed = true;
        __s_close(sockfd);
        return {};
    }
    
    unsigned int c1 = buffer[1];
    int     size = (c1 & 127);
    bool hasMask = (c1 & 128) != 0;

    char mask[4] = { 0, 0, 0, 0 };
    if (hasMask) read(sockfd, mask, 4);

    read(sockfd, buffer, size);

    string result;
    for (int i = 0; i < size; i ++) {
        buffer[i] = buffer[i] ^ mask[i % 4];
        result += buffer[i];
    }

    return result;
}

const int MAX_MSG_SIZE = 125;
char c_sbf[128];

void WebSocketClient::send (string &buffer, int op_code) {
    if (closed) return ;
    for (int offset = 0; offset < buffer.size(); offset += MAX_MSG_SIZE) {
        bool last = offset + MAX_MSG_SIZE >= buffer.size();
        int  rsv  = 0;

        bool mask = false;
        int  size = buffer.size() - offset;
        if (size >= MAX_MSG_SIZE) size = MAX_MSG_SIZE;

        char chr_0 = (last << 7) + (rsv << 4) + op_code;
        char chr_1 = (mask << 7) + size;

        c_sbf[0] = chr_0;
        c_sbf[1] = chr_1;

        for (int i = 0; i < size; i ++)
            c_sbf[i + 2] = buffer[offset + i];
        
        __s_send(sockfd, c_sbf, size + 2, 0);
    }
}

void WebSocketClient::close (string message) {
    if (closed) return ;
    send(message, 8);

    __s_close(sockfd);
    closed = true;
}

bool WebSocketClient::isClosed () {
    return closed;
}
