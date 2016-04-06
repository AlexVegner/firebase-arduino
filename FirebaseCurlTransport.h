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

#include <curl/curl.h>
#include <string>
#include <cassert>
#include <cstring>

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

class FirebaseCurlTransport {
 public:
  FirebaseCurlTransport() {
    curl_ = curl_easy_init();
    assert(curl_);
  }
  ~FirebaseCurlTransport() {
    curl_easy_cleanup(curl_);
  }

  int write(const FirebaseGet& req) {
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_URL, req.url());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteFunction);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buf_);
    return curl_easy_perform(curl_);
  }
  int write(const FirebasePost& req) {
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl_, CURLOPT_URL, req.url());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteFunction);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buf_);
    return 0;
  }
  int write(const std::string& body) {
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body.c_str());
    return curl_easy_perform(curl_);
  }
  int read(std::string* out) {
    *out = buf_;
  }
 private:
  CURL *curl_;
  std::string buf_;
};

#endif // FIREBASE_CURL_H_
