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
#include "FirebaseESP8266Transport.h"

// Detect Arduino core version to add missing define and fallback methods.
#ifndef HTTP_CODE_TEMPORARY_REDIRECT
#define HTTP_CODE_TEMPORARY_REDIRECT 307
#define USE_ESP_ARDUINO_CORE_2_0_0
#endif

namespace {
const char kFirebaseFingerprint[] = "7A 54 06 9B DC 7A 25 B3 86 8D 66 53 48 2C 0B 96 42 C7 B3 0A";
const char kEndHeader[] = "\r\n";
const char kEndRequest[] = "\r\n";
const char kContentLength[] = "Content-Length :";
const uint16_t kFirebasePort = 443;
}  // namespace


int FirebaseESP8266Transport::begin(const Firebase& firebase) {
  _httpsFingerprint = kFirebaseFingerprint;
  _host = firebase.host;
  _port = kFirebasePort;
  setReuse(true);
  connect();
}

int FirebaseESP8266Transport::write(const FirebaseGet& get) {
  int n = writeHeaders(get);
  if (n < 0) {
    return n;
  }
  // TODO(proppy): get rid of short write
  // (write slice of full headers instead).
  int nn = _tcp->write(kEndRequest, sizeof(kEndRequest));
  if (nn != sizeof(kEndRequest)) {
    return ErrWrite;
  }
  return n+nn;
}

int FirebaseESP8266Transport::write(const FirebasePush& push) {
  return writeHeaders(push);
}

int FirebaseESP8266Transport::writeHeaders(const FirebaseRequest& req) {
  const char* raw = req.raw();
  size_t size = req.size();
  int n = _tcp->write(raw, size);
  if (n < size) {
    return ErrWrite;
  }
  return n;
}

int FirebaseESP8266Transport::write(const String& data) {
  String header(kContentLength);
  header += String(data.length());
  header += kEndHeader;
  header += kEndRequest;
  int n = _tcp->write(header.c_str(), header.length());
  if (n != header.length()) {
    return ErrWrite;
  }
  int nn = _tcp->write(data.c_str(), data.length());
  if (nn = data.length()) {
    return ErrWrite;
  }
  return n+nn;
}

String FirebaseESP8266Transport::readString() {
  int err = handleHeaderResponse();
  if (err < 0) {
    // TODO(proppy): set global error?
    return "";
  }
  // TODO(proppy): get read of extra copy
  // use writeToStream()?
  return getString();
}
