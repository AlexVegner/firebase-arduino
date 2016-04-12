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

// firebase-arduino is an Arduino client for Firebase.

#ifndef FIREBASE_H_
#define FIREBASE_H_

#include <string>

enum {
  ErrConnect = -1,
  ErrHandshake = -2,
  ErrWrite = -3,
  ErrRead = -4,
  ErrParse = -5
};

// force std::string instanciation
template class std::basic_string<char>;

class FirebaseRequest {
 public:
  FirebaseRequest(const char* method, const char* host, const char* auth, const char* path);
  ~FirebaseRequest();
  const char* raw() const {
    return raw_.c_str();
  }
  size_t size() const {
    return raw_.length();
  }
  const char* const host;
 protected:
  // TODO(proppy): add segmented reader method
  // instead or full copy.
  std::string raw_;
};

class FirebaseGet : public FirebaseRequest {
 public:
  FirebaseGet(const char* host, const char* auth, const char* path);
};

class FirebasePush : public FirebaseRequest {
 public:
  FirebasePush(const char* host, const char* auth, const char* path);
};

class FirebaseStream : public FirebaseRequest {
 public:
  FirebaseStream(const char* host, const char* auth, const char* path);
};

class Firebase {
 public:
  Firebase(const char* host, const char* auth = "");
  FirebaseGet get(const char* path);
  FirebasePush push(const char* path);
  FirebaseStream stream(const char* path);
  const char* const host;
  const char* const auth;
};

#endif // FIREBASE_H_
