#include "FirebaseOpenSSLTransport.h"

#include <iostream>
#include <cassert>

#include <unistd.h>

Firebase firebase("proppy-iot-button.firebaseio.com", "9g8rCCEoQ6bopuLb75VWCeQC9e06pugcrPjTeRLG");
FirebaseOpenSSLTransport transport;

int main() {
  transport.begin(firebase);

  int write_err = transport.write(firebase.get("/state"));
  assert(write_err > 0);
  std::string result;
  int read_err = transport.read(&result);
  assert(read_err >= 0);
  std::cout << "result:" << result << std::endl;

  write_err = transport.write(firebase.push("/logs"));
  assert(write_err >= 0);
  write_err = transport.write("\"data\"");
  assert(write_err >= 0);
  read_err = transport.read(&result);
  assert(read_err >= 0);
  std::cout << "result:" << result << std::endl;

  write_err = transport.write(firebase.stream("/state"));
  assert(write_err >= 0);
  while (true) {
    std::string result;
    if (transport.available()) {
      int read_err = transport.read(&result);
      assert(read_err >= 0);
      std::cout << "result:" << result << std::endl;
    } else {
      std::cout << "no data" << std::endl;
      usleep(500000);
    }
    //int read_err = transport.read(&result);
    //assert(read_err >= 0);
    //std::cout << "result:" << result << std::endl;
  }
}
