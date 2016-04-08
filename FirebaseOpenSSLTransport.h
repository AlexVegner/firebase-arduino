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

#ifndef FIREBASE_OPENSSL_TRANSPORT_H_
#define FIREBASE_OPENSSL_TRANSPORT_H_

#include <string>
#include <cassert>
#include <cstring>
#include <iostream>

#include <openssl/ssl.h>
#include "http-parser/http_parser.h"

namespace {
const char kFirebaseScheme[] = "https://";
const char kFirebasePort[] = ":443";
const char kFirebaseExt[] = ".json";
const char kFirebaseAuthQuery[] = "?auth=";
const char kFirebaseGET[] = "GET";
const char kFirebasePOST[] = "POST";
const char kFirebaseHostHeader[] = "Host: ";
const char kFirebaseHTTPVersion[] = "HTTP/1.1";
}

class FirebaseRequest {
 public:
  FirebaseRequest(const char* method, const char* host, const char* auth, const char* path) : host{host} {
    raw_ += method;
    raw_ += " ";
    raw_ += path;
    raw_ += kFirebaseExt;
    if (strlen(auth) > 0) {
      raw_ += kFirebaseAuthQuery;
      raw_ += auth;
    }
    raw_ += " ";
    raw_ += kFirebaseHTTPVersion;
    raw_ += "\r\n";
    raw_ += kFirebaseHostHeader;
    raw_ += host;
    raw_ += "\r\n";
  }
  ~FirebaseRequest() {
  }
  const char* raw() const {
    return raw_.c_str();
  }
  size_t size() const {
    return raw_.length();
  }
  const char* host;
 private:
  std::string raw_;
};

class FirebaseGet : public FirebaseRequest {
 public:
  FirebaseGet(const char* host, const char* auth, const char* path)
      : FirebaseRequest(kFirebaseGET, host, auth, path) {
  }
  ~FirebaseGet() {
  }
};

class FirebasePost : public FirebaseRequest {
 public:
  FirebasePost(const char* host, const char* auth, const char* path)
      : FirebaseRequest(kFirebasePOST, host, auth, path) {
  }
  ~FirebasePost() {
  }
};

class Firebase {
 public:
  Firebase(const char* host, const char* auth = "") : host(host), auth(auth) {
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
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ctx_ = SSL_CTX_new(TLSv1_2_client_method());
    bio_ = BIO_new_ssl_connect(ctx_);
    BIO_get_ssl(bio_, &ssl_);
    assert(ssl_ != nullptr);
    SSL_set_mode(ssl_, SSL_MODE_AUTO_RETRY);

    settings_.on_body = [](http_parser* p, const char* at, size_t length)->int{
      auto s = reinterpret_cast<std::string*>(p->data);
      std::cout << "body:" << std::string(at, length) << std::endl;
      s->append(at, length);
    };
  }
  ~FirebaseOpenSSLTransport() {
    BIO_free_all(bio_);
  }
  int begin(const Firebase& firebase) {
    connect(firebase.host);
  }

  int connect(const char* host) {
    std::string hostname(host);
    hostname += ":https";
    std::cout << hostname << std::endl;
    BIO_set_conn_hostname(bio_, hostname.c_str());
    int connect_err = BIO_do_connect(bio_);
    assert(connect_err > 0);
    int handshake_err = BIO_do_handshake(bio_);
    assert(handshake_err > 0);
  }
  int write(const FirebaseRequest& req) {
    int n = BIO_write(bio_, req.raw(), req.size());
    assert(n == req.size());
    n += BIO_puts(bio_, "\r\n");
    return n;
  }
  int write(const std::string& data) {
    return BIO_puts(bio_, data.c_str());
  }
  int read(std::string* out) {
    char buf[1024];
    out->clear();
    http_parser_init(&parser_, HTTP_RESPONSE);
    for (;;) {
      int n = BIO_read(bio_, buf, sizeof(buf));
      parser_.data = out;
      int nparsed = http_parser_execute(&parser_, &settings_, buf, n);
      std::cout << "nread:" << n << ", nparsed:" << nparsed << std::endl;
      if (http_body_is_final(&parser_)) {
        return nparsed;
      }
      //out->append(buf, n);
    }
  }
 private:
  SSL_CTX* ctx_{nullptr};
  SSL* ssl_{nullptr};
  BIO* bio_{nullptr};
  http_parser parser_;
  http_parser_settings settings_;
};

#endif // FIREBASE_OPENSSL_TRANSPORT_H_
