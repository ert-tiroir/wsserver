
/**********************************************************************************/
/* example.cpp                                                                    */
/*                                                                                */
/* This file contains a simple example on how one might broadcast and receive     */
/* data using wsserver                                                            */
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

#include <iostream>
#include <string>

using namespace std;

// Callback function when a message is processed by the server
void callback (WebSocketClient* client, string message) {
    cout << message << endl;
}

int main () {
    // Create a web socket server on port 5420
    WebSocketServer server;
    server.init( 5420 );

    while (true) {
        string buffer;
        getline(cin, buffer);

        if (buffer == "exit") break ;
        if (buffer == "send") {
            string message; getline(cin, message);

            server.broadcast(message);
        }
        if (buffer == "recv")
            server.receive(callback);
        
        if (buffer == "listen")
            while (!server.listen())
                continue ;
    }

    // close the server and destroy all the file descriptors for the clients
    server.close();
}
