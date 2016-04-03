//
// Copyright 2015 Google Inc.
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

// firebase-arduino is an Arduino client for Firebase.
// It is currently limited to the ESP8266 board family.

#ifndef FIREBASE_CURL_H_
#define FIREBASE_CURL_H_

#include "Firebase.h"
#include <curl/curl.h>
#include <string>
#include <cassert>

size_t WriteFunction(void* contents, size_t size, size_t nmemb, void* userp) {
  static_cast<std::string*>(userp)->append(static_cast<const char*>(contents),
                                           size * nmemb);
  return size * nmemb;
}

class FirebaseCurlTransport {
 public:
  FirebaseCurlTransport() {
    curl_ = curl_easy_init();
    assert(curl_);
  }
  ~FirebaseCurlTransport() {
    curl_easy_cleanup(curl_);
  }
  template<int method, size_t HOST_SIZE, size_t AUTH_SIZE, size_t PATH_SIZE>
  int write(
      const FirebaseRequest<method, HOST_SIZE, AUTH_SIZE, PATH_SIZE>& req) {
    switch(method) {
      case GET:
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
        break;
      default:
        assert(false && "unsupported method");
        break;
    }
    curl_easy_setopt(curl_, CURLOPT_URL, req.url);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteFunction);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buf_);
    CURLcode res = curl_easy_perform(curl_);
    return res;
  }
  int read(std::string* out) {
    *out = buf_;
  }
 private:
  CURL *curl_;
  std::string buf_;
};

#endif // FIREBASE_CURL_H_
