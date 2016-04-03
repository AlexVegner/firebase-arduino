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

#ifndef FIREBASE_H_
#define FIREBASE_H_

#ifndef FIREBASE_URL_SIZE
#define FIREBASE_URL_SIZE 512
#endif // FIREBASE_URL_SIZE

#include <cstring>

namespace {
const char kFirebaseScheme[] = "https://";
const char kFirebaseFingerprint[] = "7A 54 06 9B DC 7A 25 B3 86 8D 66 53 48 2C 0B 96 42 C7 B3 0A";
const char kFirebasePort[] = ":443";
const char kFirebaseExt[] = ".json";
const char kFirebaseAuthQuery[] = "?auth=";
template<size_t SRC_SIZE>
void cat(char*& dst, const char (&src)[SRC_SIZE]){
  memcpy(dst, src, sizeof(src));
  dst+=sizeof(src)-1;
}
}  // namespace

enum HttpMethod {
  GET,
};
template<int method, size_t HOST_SIZE, size_t AUTH_SIZE, size_t PATH_SIZE>
struct FirebaseRequest {
  FirebaseRequest(const char (&host)[HOST_SIZE],
                  const char (&auth)[AUTH_SIZE],
                  const char (&path)[PATH_SIZE],
                  const char* data) {
    char* p = url;
    cat(p, kFirebaseScheme);
    cat(p, host);
    cat(p, kFirebasePort);
    cat(p, path);
    cat(p, kFirebaseExt);
    cat(p, kFirebaseAuthQuery);
    cat(p, auth);
  }
  const char* data;
  char url[sizeof(kFirebaseScheme)-1+
           HOST_SIZE-1+
           sizeof(kFirebasePort)-1+
           PATH_SIZE-1+
           sizeof(kFirebaseExt)-1+
           sizeof(kFirebaseAuthQuery)-1+
           AUTH_SIZE];
};

template<size_t HOST_SIZE, size_t AUTH_SIZE, size_t PATH_SIZE>
struct FirebaseGetRequest
    : FirebaseRequest<GET, HOST_SIZE, AUTH_SIZE, PATH_SIZE> {
  FirebaseGetRequest(const char (&host)[HOST_SIZE],
              const char (&auth)[AUTH_SIZE],
              const char (&path)[PATH_SIZE])
      : FirebaseRequest<GET, HOST_SIZE, AUTH_SIZE, PATH_SIZE>{host, auth, path, nullptr} {}
};

template<size_t HOST_SIZE, size_t AUTH_SIZE, size_t PATH_SIZE>
FirebaseGetRequest<HOST_SIZE, AUTH_SIZE, PATH_SIZE> FirebaseGet(
    const char (&host)[HOST_SIZE],
    const char (&auth)[AUTH_SIZE],
    const char (&path)[PATH_SIZE]) {
  return FirebaseGetRequest<HOST_SIZE, AUTH_SIZE, PATH_SIZE>(
      host, auth, path);
}


template<size_t HOST_SIZE, size_t AUTH_SIZE>
struct FirebaseHost {
  FirebaseHost(const char (&host_)[HOST_SIZE],
           const char (&auth_)[AUTH_SIZE])
      : host(host_), auth(auth_) {}
  template<size_t PATH_SIZE>
  FirebaseGetRequest<HOST_SIZE, AUTH_SIZE, PATH_SIZE>
  get(const char (&path)[PATH_SIZE]) {
    return FirebaseGetRequest<HOST_SIZE, AUTH_SIZE, PATH_SIZE>{
      host, auth, path
    };
  }
  const char (&host)[HOST_SIZE];
  const char (&auth)[AUTH_SIZE];
};

template<size_t HOST_SIZE, size_t AUTH_SIZE>
FirebaseHost<HOST_SIZE, AUTH_SIZE> Firebase(
    const char (&host)[HOST_SIZE],
    const char (&auth)[AUTH_SIZE]) {
  return FirebaseHost<HOST_SIZE, AUTH_SIZE>(
      host, auth);
}

#endif // FIREBASE_H_
