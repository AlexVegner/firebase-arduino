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

#include "Firebase.h"

#include <cstring>

namespace {
const char kScheme[] = "https://";
const char kPort[] = ":443";
const char kExtJSON[] = ".json";
const char kAuthQuery[] = "?auth=";
const char kGET[] = "GET";
const char kPOST[] = "POST";
const char kHostHeader[] = "Host: ";
const char kAcceptStreamHeader[] = "Accept: text/event-stream\r\n";
const char kHTTPVersion[] = "HTTP/1.1";
const char kSpace[] = " ";
const char kEndLine[] = "\r\n";
const char kEndHeader[] = "\r\n";
}  // namespace

FirebaseRequest::FirebaseRequest(const char* method, const char* host, const char* auth, const char* path) : host{host} {
  raw_ += method;
  raw_ += " ";
  raw_ += path;
  raw_ += kExtJSON;
  if (strlen(auth)) {
    raw_ += kAuthQuery;
    raw_ += auth;
  }
  raw_ += " ";
  raw_ += kHTTPVersion;
  raw_ += kEndLine;
  raw_ += kHostHeader;
  raw_ += host;
  raw_ += kEndHeader;
}

FirebaseRequest::~FirebaseRequest() {
}

FirebaseGet::FirebaseGet(const char* host, const char* auth, const char* path)
    : FirebaseRequest(kGET, host, auth, path) {
}

FirebasePush::FirebasePush(const char* host, const char* auth, const char* path)
    : FirebaseRequest(kPOST, host, auth, path) {
}

FirebaseStream::FirebaseStream(const char* host, const char* auth, const char* path)
    : FirebaseRequest(kGET, host, auth, path) {
  raw_ += kAcceptStreamHeader;
}

Firebase::Firebase(const char* host, const char* auth) : host(host), auth(auth) {
}

FirebaseGet Firebase::get(const char* path) {
  return FirebaseGet(host, auth, path);
}

FirebasePush Firebase::push(const char* path) {
  return FirebasePush(host, auth, path);
}

FirebaseStream Firebase::stream(const char* path) {
  return FirebaseStream(host, auth, path);
}
