#include "FirebaseCurlTransport.h"

#include <iostream>

Firebase firebase("proppy-iot-button.firebaseio.com", "KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u");
FirebasePost postFoo = firebase.post("/foo");
FirebaseCurlTransport curl;

int main() {
  curl.write(firebase.post("/foo"));
  curl.write("{\"some\":\"data\"}");
  std::string result;
  curl.read(&result);
  std::cout << result << std::endl;

  curl.write(firebase.get("/"));
  curl.read(&result);
  std::cout << result << std::endl;
}
