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

#include "Firebase.h"

#include <openssl/ssl.h>
#include <http_parser.h>

#include <string>

class FirebaseOpenSSLTransport {
 public:
  FirebaseOpenSSLTransport();
  ~FirebaseOpenSSLTransport();
  int begin(const Firebase& firebase);
  int connect(const char* host);
  int write(const FirebaseGet& get);
  int write(const FirebasePush& push);
  int write(const FirebaseStream& stream);
  int write(const std::string& data);
  int read(std::string* out);
 private:
  int writeHeaders(const FirebaseRequest& req);
  SSL_CTX* ctx_{nullptr};
  SSL* ssl_{nullptr};
  BIO* bio_{nullptr};
  http_parser parser_;
  http_parser_settings settings_;
  bool streaming_;
};

#endif // FIREBASE_OPENSSL_TRANSPORT_H_
