#include "FirebaseCurlTransport.h"
#include "Firebase.h"

#include <iostream>

int main() {
  Firebase firebase("https://proppy-iot-button.firebaseio.com", "KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u");
  FirebaseGet getLogs = firebase.get("/logs");

  FirebaseCurlTransport transport;
  transport.write(getLogs);
  std::string result;
  transport.read(&result);
  std::cout << result << std::endl;
}
