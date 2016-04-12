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

// FirebasePush_ESP8266 is a sample that push a new timestamp to firebase
// on each reset.

#include <FirebaseESP8266.h>

// create firebase client.
Firebase firebase = Firebase("example.firebaseio.com", "secret_or_token");
// create transport
FirebaseESP8266Transport transport;

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin("SSID", "PASSWORD");
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // connect to firebase
  transport.begin(firebase);
  
  // add a new entry.
  transport.write(firebase.push("/logs"));
  int n = transport.write("{\".sv\": \"timestamp\"}");
  if (n < 0) {
      Serial.println("Firebase push failed");
      return;
  }
  String result;
  transport.read(&result);
  Serial.println(result);

  // get all entries.
  n = transport.write(firebase.get("/logs"));
  if (n < 0) {
      Serial.println("Firebase get failed");
      return;
  }
  transport.read(&result);
  Serial.println(result);
}

void loop() {
}
