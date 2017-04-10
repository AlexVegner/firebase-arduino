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

#include "FirebaseArduino.h"

namespace {
  const char* kApplicationType = "application/json";
  String makeFirebaseURI(const char* path, const char* auth) {
    String uri;
    if (path[0] != '/') {
      uri = "/";
    }
    uri += path;
    uri += ".json";
    if (strlen(auth) > 0) {
      uri += "?auth=";
      uri += auth;
    }
    return uri;
  }
}  // namespace

FirebaseArduino::FirebaseArduino(Client& client) : client_(client) {}

void FirebaseArduino::begin(const char* host, const char* auth) {
  http_.begin(client_, host, 443);
  http_.connectionKeepAlive();
  auth_ = auth;
}

String FirebaseArduino::pushInt(const char* path, int value) {
  return push(path, value);
}

String FirebaseArduino::pushFloat(const char* path, float value) {
  return push(path, value);
}

String FirebaseArduino::pushBool(const char* path, bool value) {
  return push(path, value);
}

String FirebaseArduino::pushString(const char* path, const char* value) {
  JsonVariant json(value);
  return push(path, json);
}

String FirebaseArduino::push(const char* path, const JsonVariant& value) {
  String uri = makeFirebaseURI(path, auth_);
  String buf;
  value.printTo(buf);
  int err = http_.post(uri.c_str(), kApplicationType, buf.c_str());
  if (err != 0) {
    error_ = FirebaseError(err, "HTTP request failed");
    return "";
  }
  int statusCode = http_.responseStatusCode();
  if (statusCode != 200) {
    error_ = FirebaseError(statusCode, "PUT request failed");
    return "";
  }
  return http_.responseBody();
}

void FirebaseArduino::setInt(const char* path, int value) {
  set(path, value);
}

void FirebaseArduino::setFloat(const char* path, float value) {
  set(path, value);
}

void FirebaseArduino::setBool(const char* path, bool value) {
  set(path, value);
}

void FirebaseArduino::setString(const char* path, const char* value) {
  JsonVariant json(value);
  set(path, json);
}

void FirebaseArduino::set(const char* path, const JsonVariant& value) {
  String uri = makeFirebaseURI(path, auth_);
  String buf;
  value.printTo(buf);
  int err = http_.put(uri.c_str(), kApplicationType, buf.c_str());
  if (err != 0) {
    error_ = FirebaseError(err, "HTTP request failed");
    return;
  }
  int statusCode = http_.responseStatusCode();
  if (statusCode != 200) {
    error_ = FirebaseError(statusCode, "POST request failed");
  }
  http_.responseBody();
}

FirebaseObject FirebaseArduino::get(const char* path) {
  String uri = makeFirebaseURI(path, auth_);
  int err = http_.get(uri.c_str());
  if (err != 0) {
    error_ = FirebaseError(err, "HTTP request failed");
    return FirebaseObject{""};
  }
  int statusCode = http_.responseStatusCode();
  if (statusCode != 200) {
    error_ = FirebaseError(err, "GET request failed");
    return FirebaseObject{""};
  }
  return FirebaseObject(http_.responseBody());
}

int FirebaseArduino::getInt(const char* path) {
  return get(path).getInt();
}


float FirebaseArduino::getFloat(const char* path) {
  return get(path).getFloat();
}

String FirebaseArduino::getString(const char* path) {
  return get(path).getString();
}

bool FirebaseArduino::getBool(const char* path) {
  return get(path).getBool();
}

void FirebaseArduino::remove(const char* path) {
  String uri = makeFirebaseURI(path, auth_);
  int err = http_.del(uri.c_str());
  if (err != 0) {
    error_ = FirebaseError(err, "HTTP request failed");
    return;
  }
  int statusCode = http_.responseStatusCode();
  if (statusCode != 200) {
    error_ = FirebaseError(err, "DELETE request failed");
  }
  http_.responseBody();
}

bool FirebaseArduino::success() {
  return error_.code() == 0;
}

bool FirebaseArduino::failed() {
  return error_.code() != 0;
}

const FirebaseError& FirebaseArduino::error() {
  return error_;
}
