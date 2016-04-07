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

namespace {
const char kFirebaseScheme[] = "https://";
const char kFirebasePort[] = ":443";
const char kFirebaseExt[] = ".json";
const char kFirebaseAuthQuery[] = "?auth=";
}

class FirebaseGet {
 public:
  FirebaseGet(const char* host, const char* auth, const char* path) : host(host), auth(auth), path(path) {
  }
  ~FirebaseGet() {
  }
  const char* const host;
  const char* const auth;
  const char* const path;
};

class FirebasePost {
 public:
  FirebasePost(const char* host, const char* auth, const char* path) : host(host), auth(auth), path(path) {
  }
  ~FirebasePost() {
  }
  const char* const host;
  const char* const auth;
  const char* const path;
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
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ctx_ = SSL_CTX_new(TLSv1_2_client_method());
    bio_ = BIO_new_ssl_connect(ctx_);
    BIO_get_ssl(bio_, &ssl_);
    assert(ssl_ != nullptr);
    SSL_set_mode(ssl_, SSL_MODE_AUTO_RETRY);
  }
  ~FirebaseOpenSSLTransport() {
    BIO_free_all(bio_);
  }
  int begin(const Firebase& firebase) {
    BIO_set_conn_hostname(bio_, "proppy-iot-button.firebaseio.com:https");
    int connect_err = BIO_do_connect(bio_);
    assert(connect_err > 0);
    int handshake_err = BIO_do_handshake(bio_);
    assert(handshake_err > 0);
    connected_ = true;
  }
  int write(const FirebaseGet& req) {
    assert(connected_);
    int n = BIO_puts(bio_, "GET /foo.json?auth=KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u HTTP/1.1\r\n");
    n += BIO_puts(bio_, "Host: proppy-iot-button.firebaseio.com\r\n");
    n += BIO_puts(bio_, "Connect: Close\r\n\r\n");
    return n;
  }
  int write(const FirebasePost& req) {
    assert(connected_);
  }
  int write(const std::string& data) {
    assert(connected_);
    return BIO_puts(bio_, data.c_str());
  }
  int read(std::string* out) {
    assert(connected_);
    char buf[1024];
    for (;;) {
      int n = BIO_read(bio_, buf, sizeof(buf));
      std::cout << buf << std::endl;
      //out->append(buf, n);
    }
  }
 private:
  SSL_CTX* ctx_{nullptr};
  SSL* ssl_{nullptr};
  BIO* bio_{nullptr};
  bool connected_{false};
};

#endif // FIREBASE_OPENSSL_TRANSPORT_H_
