#include "FirebaseCurlTransport.h"
#include "Firebase.h"

#include <iostream>

auto firebase = Firebase("proppy-iot-button.firebaseio.com", "KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u");

int main() {
  FirebaseCurlTransport transport;
  transport.write(firebase.get("/"));
  std::string result;
  transport.read(&result);
  std::cout << result << std::endl;
}
