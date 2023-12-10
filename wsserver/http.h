
/**********************************************************************************/
/* wsserver/http.h                                                                */
/*                                                                                */
/* This file contains the details for tiny implementations of http request and    */
/* that work in the context of this tiny web socket server                        */
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

#include <string>
#include <map>

using namespace std;

struct HttpRequest {
private:
    string method, path, version;

    map<string, string> headers;
public:
    HttpRequest (string buffer);

    string getMethod  ();
    string getPath    ();
    string getVersion ();

    bool   hasHeader (string key);
    string getHeader (string key);

    bool isWebSocketRequest ();
};

struct HttpResponse {
private:
    string version;
    int    code;
    string message;

    map<string, string> headers;
public:
    HttpResponse (HttpRequest request, int code, string message);

    void setHeader (string key, string value);
    string as_string ();
};
