#include "FirebaseOpenSSLTransport.h"

#include <iostream>
#include <cassert>

Firebase firebase("proppy-iot-button.firebaseio.com");
FirebaseOpenSSLTransport transport;
FirebasePush pushFoo = firebase.push("/foo");

int main() {
  transport.begin(firebase);
  int write_err = transport.write(firebase.get("/"));
  assert(write_err > 0);

  std::string result;
  int read_err = transport.read(&result);
  assert(read_err > 0);
  std::cout << result << std::endl;

  write_err = transport.write(firebase.get("/state"));
  transport.read(&result);
  std::cout << result << std::endl;

  write_err = transport.write(pushFoo);
  transport.write("{\"some\":\"data\"}");
  transport.read(&result);
  std::cout << result << std::endl;
}
