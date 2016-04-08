#include "FirebaseOpenSSLTransport.h"

#include <iostream>

Firebase firebase("proppy-iot-button.firebaseio.com");
FirebaseOpenSSLTransport transport;
FirebasePost postFoo = firebase.post("/foo");

int main() {
  transport.begin(firebase);
  int err = transport.write(firebase.get("/"));
  std::cout << err << std::endl;
  std::string result;
  transport.read(&result);
  std::cout << result << std::endl;

  err = transport.write(firebase.get("/state"));
  std::cout << err << std::endl;
  transport.read(&result);
  std::cout << result << std::endl;

  //transport.write(postFoo);
  //transport.write("{\"some\":\"data\"}");
  //transport.read(&result);
  //std::cout << result << std::endl;
}
