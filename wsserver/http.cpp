
/**********************************************************************************/
/* wsserver/http.h                                                                */
/*                                                                                */
/* This file contains the implementations of the http requests and responses      */
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

#include <wsserver/http.h>

#include <vector>

int indexOf (string buffer, string delimiter) {
    for (int i = 0; i + delimiter.size() - 1 < buffer.size(); i ++) {
        bool valid = true;
        for (int j = 0; j < delimiter.size(); j ++)
            valid &= buffer[i + j] == delimiter[j];
        
        if (valid) return i;
    }

    return -1;
}
vector<string> split (string buffer, string delimiter, int limit = -1) {
    vector<string> res;

    int index;
    while ((index = indexOf(buffer, delimiter)) != -1 && limit != 0) {
        res.push_back( buffer.substr(0, index) );
        buffer = buffer.substr(index + delimiter.size());
        limit --;
    }

    res.push_back(buffer);

    return res;
}
#include <iostream>

HttpRequest::HttpRequest (string buffer) {
    vector<string> lines = split(buffer, "\r\n");

    vector<string> data = split(lines [0], " ");

    method  = data.size() <= 0 ? "" : data[0];
    path    = data.size() <= 1 ? "" : data[1];
    version = data.size() <= 2 ? "" : data[2];

    for (int lineId = 1; lineId < lines.size(); lineId ++) {
        if (lines[lineId] == "") break ;
        vector<string> md = split(lines[lineId], ": ", 1);

        headers.insert({ md[0], md[1] });
    }
}

string HttpRequest::getMethod  () { return method;  }
string HttpRequest::getPath    () { return path;    }
string HttpRequest::getVersion () { return version; }

bool HttpRequest::hasHeader (string key) {
    return headers.find(key) != headers.end();
}
string HttpRequest::getHeader (string key) {
    if (!hasHeader(key)) return "";
    return (*headers.find(key)).second;
}

/************************************************************************/
/* SUB PROTOCOLS                                                        */
/************************************************************************/

bool HttpRequest::isWebSocketRequest () {
    return getMethod () == "GET"
        && getVersion() == "HTTP/1.1"
        && hasHeader("Host")
        && getHeader("Upgrade") == "websocket"
        && indexOf(getHeader("Connection"), "Upgrade") != -1
        && hasHeader("Sec-WebSocket-Key")
        && hasHeader("Sec-WebSocket-Version");
}






HttpResponse::HttpResponse (HttpRequest request, int code, string message) {
    this->version = request.getVersion();
    this->code    = code;
    this->message = code;
}

void HttpResponse::setHeader (string key, string value) {
    if (headers.find(key) != headers.end()) headers.erase(key);
    headers.insert({ key, value });
}

const string line_sep  = "\r\n";

string HttpResponse::as_string () {
    string result = version + " " + to_string(code) + " " + message + line_sep;

    for (const auto &header : headers) {
        result += header.first + ": " + header.second + line_sep;
    }

    result += line_sep;

    return result;
}