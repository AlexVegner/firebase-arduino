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

size_t WriteFunction(void* contents, size_t size, size_t nmemb, void* userp) {
  static_cast<std::string*>(userp)->append(static_cast<const char*>(contents),
                                           size * nmemb);
  return size * nmemb;
}

class FirebaseCurlTransport {
 public:
  int write(const FirebaseGet& get) {
    CURL *curl = curl_easy_init();
    if(curl) {
      CURLcode res;
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
      curl_easy_setopt(curl, CURLOPT_URL, get.URL());
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

      //FirebaseResult result;
      //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunction);
      //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body_);

      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
      return 0;
    }
    return -1;
  }
  int read(const std::string& out) {
  }
};

#endif // FIREBASE_CURL_H_
