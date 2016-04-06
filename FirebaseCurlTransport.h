//
// Copyright 2016 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef FIREBASE_CURL_H_
#define FIREBASE_CURL_H_

#include <string>
#include <cassert>
#include <cstring>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace {
const char kFirebaseScheme[] = "https://";
const char kFirebasePort[] = ":443";
const char kFirebaseExt[] = ".json";
const char kFirebaseAuthQuery[] = "?auth=";
size_t WriteFunction(void* contents, size_t size, size_t nmemb, void* userp) {
  static_cast<std::string*>(userp)->append(static_cast<const char*>(contents),
                                           size * nmemb);
  return size * nmemb;
}
void cat(char*& dst, const char *src, size_t n) {
  memcpy(dst, src, n);
  dst+=n;
}
}

class FirebaseGet {
 public:
  FirebaseGet(const char* host, const char* auth, const char* path) : host(host), auth(auth), path(path) {
    size_t nhost = strlen(host);
    size_t npath = strlen(path);
    size_t nauth = strlen(auth);
    size_t nurl = sizeof(kFirebaseScheme)-1 + nhost + sizeof(kFirebasePort)-1 +
        npath + sizeof(kFirebaseExt)-1 + sizeof(kFirebaseAuthQuery)-1 + nauth + 1;
    char* p = url_ = new char[nurl]();
    cat(p, kFirebaseScheme, sizeof(kFirebaseScheme)-1);
    cat(p, host, nhost);
    cat(p, kFirebasePort, sizeof(kFirebasePort)-1);
    cat(p, path, npath);
    cat(p, kFirebaseExt, sizeof(kFirebaseExt)-1);
    cat(p, kFirebaseAuthQuery, sizeof(kFirebaseAuthQuery)-1);
    cat(p, auth, nauth);
    *p = '\0';
  }
  ~FirebaseGet() {
    delete[](url_);
  }
  const char* url() const {
    return url_;
  }
  const char* const host;
  const char* const auth;
  const char* const path;
 private:
  char* url_;
};

class FirebasePost {
 public:
  FirebasePost(const char* host, const char* auth, const char* path) : host(host), auth(auth), path(path) {
    size_t nhost = strlen(host);
    size_t npath = strlen(path);
    size_t nauth = strlen(auth);
    size_t nurl = sizeof(kFirebaseScheme)-1 + nhost + sizeof(kFirebasePort)-1 +
        npath + sizeof(kFirebaseExt)-1 + sizeof(kFirebaseAuthQuery)-1 + nauth + 1;
    char* p = url_ = new char[nurl]();
    cat(p, kFirebaseScheme, sizeof(kFirebaseScheme)-1);
    cat(p, host, nhost);
    cat(p, kFirebasePort, sizeof(kFirebasePort)-1);
    cat(p, path, npath);
    cat(p, kFirebaseExt, sizeof(kFirebaseExt)-1);
    cat(p, kFirebaseAuthQuery, sizeof(kFirebaseAuthQuery)-1);
    cat(p, auth, nauth);
    *p = '\0';
  }
  ~FirebasePost() {
    delete[](url_);
  }
  const char* url() const {
    return url_;
  }
  const char* const host;
  const char* const auth;
  const char* const path;
 private:
  char* url_;
};

class Firebase {
 public:
  Firebase(const char* host, const char* auth) : host(host), auth(auth) {
  }
  FirebaseGet get(const char* path) {
    return FirebaseGet(host, auth, path);
  }
  FirebasePost post(const char* path) {
    return FirebasePost(host, auth, path);
  }
  const char* const host;
  const char* const auth;
};

class FirebaseOpenSSLTransport {
 public:
  FirebaseOpenSSLTransport() {
  }
  ~FirebaseOpenSSLTransport() {
  }
  int begin(const Firebase& firebase) {
    return connect(firebase.host);
  }
  int write(const FirebaseGet& req) {
    assert(sock_ != 0);
    write("GET /foo.json?auth=KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u HTTP/1.1\r\n");
    write("Host: proppy-iot-button.firebaseio.com\r\n");
    write("User-Agent: firebase-iot\r\n");
    write("Connection: Close\r\n\r\n");
  }
  int write(const FirebasePost& req) {
    assert(sock_ != 0);
  }
  int write(const std::string& data) {
    assert(sock_ != 0);
    int n = send(sock_, data.c_str(), data.length(), 0);
    assert(n == data.length());
    std::cout << n << std::endl;
  }
  int read(std::string* out) {
    assert(sock_ != 0);
    char buf[256];
    memset(buf, 0, 256);
    int n = recv(sock_, buf, sizeof(buf), 0);
    std::cout << buf << std::endl;
    out->append(buf, n);
  }
 private:
  int connect(const char* host) {
    addrinfo *result;
    addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int getaddrinfo_err =  getaddrinfo(host, "443", NULL, &result);
    assert(getaddrinfo_err == 0);
    sock_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    assert(sock_ != -1);
    int connect_err = ::connect(sock_, result->ai_addr, result->ai_addrlen);
    assert(connect_err == 0);
    freeaddrinfo(result);
  }
  int sock_;
};

#endif // FIREBASE_CURL_H_
