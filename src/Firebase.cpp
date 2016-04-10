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
const char kHTTPVersion[] = "HTTP/1.1";
const char kSpace[] = " ";
const char kEndLine[] = "\r\n";
const char kEndHeader[] = "\r\n";
char* cat(char *out, const char* head, size_t nhead) {
  memcpy(out, head, nhead);
  return out+nhead;
}
template<typename... Ts>
char* cat(char* out, const char* head, size_t nhead, Ts... tail) {
  cat(cat(out, head, nhead), tail...);
}
template<size_t N>
constexpr size_t n(const char (&s)[N]) {
  return N-1;
}
}  // namespace

FirebaseRequest::FirebaseRequest(const char* method, const char* host, const char* auth, const char* path) : host{host} {
  size_t nmethod = std::strlen(method); // TODO(proppy): use const value.
  size_t nhost = std::strlen(host);
  size_t nauth = std::strlen(auth);
  size_t npath = std::strlen(path);
  size_t nraw = nmethod + n(kSpace) + npath + n(kExtJSON);
  if (nauth) {
    nraw += n(kAuthQuery)+nauth;
  }
  nraw += n(kSpace) + n(kHTTPVersion) + n(kEndLine) + n(kHostHeader) + nhost + n(kEndHeader);
  raw_ = new char[nraw+1];
  size_ = nraw;
  char* p = cat(raw_, method, nmethod, kSpace, n(kSpace), path, npath, kExtJSON, n(kExtJSON));
  if (nauth) {
    p = cat(p, kAuthQuery, n(kAuthQuery), auth, nauth);
  }
  p = cat(p,
      kSpace, n(kSpace), kHTTPVersion, n(kHTTPVersion), kEndLine, n(kEndLine),
      kHostHeader, n(kHostHeader), host, nhost, kEndHeader, n(kEndHeader));
  *p = '\0';
}

FirebaseRequest::~FirebaseRequest() {
  delete[] raw_;
}

FirebaseGet::FirebaseGet(const char* host, const char* auth, const char* path)
    : FirebaseRequest(kGET, host, auth, path) {
}

FirebasePush::FirebasePush(const char* host, const char* auth, const char* path)
    : FirebaseRequest(kPOST, host, auth, path) {
}

Firebase::Firebase(const char* host, const char* auth) : host(host), auth(auth) {
}

FirebaseGet Firebase::get(const char* path) {
  return FirebaseGet(host, auth, path);
}

FirebasePush Firebase::push(const char* path) {
  return FirebasePush(host, auth, path);
}
