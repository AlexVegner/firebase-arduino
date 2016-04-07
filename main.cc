#include "FirebaseOpenSSLTransport.h"

#include <iostream>

Firebase firebase("proppy-iot-button.firebaseio.com", "KqfUj6MGR1SLjeudfgWdPskmukiW1Fw7d0LT4S3u");
FirebaseOpenSSLTransport transport;
FirebasePost postFoo = firebase.post("/foo");

int main() {
  transport.begin(firebase);
  int err = transport.write(firebase.get("/"));
  std::cout << err << std::endl;
  std::string result;
  transport.read(&result);
  std::cout << result << std::endl;

  //sock.write(firebase.post("/foo"));
  //sock.write("{\"some\":\"data\"}");
  //std::string result;
  //sock.read(&result);
}
