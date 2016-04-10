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

class FirebaseRequest {
 public:
  FirebaseRequest(const char* method, const char* host, const char* auth, const char* path);
  ~FirebaseRequest();
  const char* raw() const {
    return raw_;
  }
  size_t size() const {
    return size_;
  }
  const char* const host;
 private:
  // TODO(proppy): add segmented reader method
  // instead or full copy.
  char* raw_;
  int size_;
};

class FirebaseGet : public FirebaseRequest {
 public:
  FirebaseGet(const char* host, const char* auth, const char* path);
};

class FirebasePush : public FirebaseRequest {
 public:
  FirebasePush(const char* host, const char* auth, const char* path);
};

class Firebase {
 public:
  Firebase(const char* host, const char* auth = "");
  FirebaseGet get(const char* path);
  FirebasePush push(const char* path);
  const char* const host;
  const char* const auth;
};

#endif // FIREBASE_H_
