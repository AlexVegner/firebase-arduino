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

#ifndef firebase_error_h
#define firebase_error_h

class FirebaseError {
 public:
  // Make it explicit that the empty constructor mean no error.
  static FirebaseError OK() {
    return FirebaseError();
  }

  FirebaseError() {}
  FirebaseError(int code, const String& message) : code_(code), message_(message) {
  }

  operator String() const { return message_ + ": " + code_; }
  int code() const { return code_; }
  const String& message() const { return message_; }

 private:
  int code_ = 0;
  String message_ = "";
};

#endif  //firebase_error_h
