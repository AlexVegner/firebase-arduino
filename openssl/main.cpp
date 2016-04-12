#include "FirebaseOpenSSLTransport.h"

#include <iostream>
#include <cassert>

Firebase firebase("proppy-iot-button.firebaseio.com");
FirebaseOpenSSLTransport transport;

int main() {
  transport.begin(firebase);

  int write_err = transport.write(firebase.stream("/state"));
  assert(write_err > 0);
  std::string result;
  while(true) {
    std::cout << "streaming" << std::endl;
    int stream_err = transport.read(&result);
    assert(stream_err > 0);
    std::cout << "result: " << result << std::endl;
  }
}
