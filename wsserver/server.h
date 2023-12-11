
/**********************************************************************************/
/* wsserver/server.h                                                              */
/*                                                                                */
/* This file contains the details of the WebSocketServer structure                */
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

#pragma once

#include <netinet/in.h>
#include <vector>
#include <wsserver/client.h>

using namespace std;

typedef void (*ReceiveCallback) (WebSocketClient* client, string message);

struct WebSocketServer {
private:
    int sockfd;

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    vector<WebSocketClient*> clients;

    int max_packet_size;
public:
    void init   (int port, int max_packet_size = 1 << 28);
    bool listen ();
    void close  (string message = "");

    void broadcast (string &message);
    void receive   (ReceiveCallback callback);

    int              clientCount ();
    WebSocketClient* getClient   (int id);
};
