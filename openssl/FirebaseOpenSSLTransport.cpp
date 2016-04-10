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

#include "FirebaseOpenSSLTransport.h"

#include <cassert>

namespace {
const char kContentLengthHeader[] = "Content-Length: ";
const size_t kReadBufferSize = 1024;
}  // namespace

FirebaseOpenSSLTransport::FirebaseOpenSSLTransport() {
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  ctx_ = SSL_CTX_new(TLSv1_2_client_method());
  bio_ = BIO_new_ssl_connect(ctx_);
  BIO_get_ssl(bio_, &ssl_);
  assert(ssl_ != nullptr);
  SSL_set_mode(ssl_, SSL_MODE_AUTO_RETRY);
}

FirebaseOpenSSLTransport::~FirebaseOpenSSLTransport() {
  BIO_free_all(bio_);
}

int FirebaseOpenSSLTransport::begin(const Firebase& firebase) {
  return connect(firebase.host);
}

int FirebaseOpenSSLTransport::connect(const char* host) {
  std::string hostname(host);
  hostname += ":https";
  BIO_set_conn_hostname(bio_, hostname.c_str());
  int connect_err = BIO_do_connect(bio_);
  if (connect_err <= 0) {
    return ErrConnect;
  }
  int handshake_err = BIO_do_handshake(bio_);
  if (handshake_err <= 0) {
    return ErrHandshake;
  }
}

int FirebaseOpenSSLTransport::write(const FirebaseGet& get) {
  int n = writeHeaders(get);
  if (n <= 0) {
    return n;
  }
  int nn = BIO_puts(bio_, "\r\n"); // no body
  if (nn <= 0) {
    return ErrWrite;
  }
  return n+nn;
}

int FirebaseOpenSSLTransport::write(const FirebasePush& push) {
  return writeHeaders(push); // next write is body
}

int FirebaseOpenSSLTransport::writeHeaders(const FirebaseRequest& req) {
  // TODO(proppy): drain response body
  const char* raw = req.raw();
  size_t size = req.size();
  for (int i = 0; i < size;) {
    int n = BIO_write(bio_, raw+i, size-i);
    if (n <= 0) {
      return ErrWrite;
    }
    i += n;
  }
  return size;
}

int FirebaseOpenSSLTransport::write(const std::string& data) {
  std::string header(kContentLengthHeader);
  header += std::to_string(data.length());
  header += "\r\n\r\n"; // end request
  int n = BIO_puts(bio_, header.c_str());
  if (n <= 0) {
    return ErrWrite;
  }
  int nn = BIO_puts(bio_, data.c_str()); // write body
  if (nn <= 0) {
    return ErrWrite;
  }
  return n+nn;
}

int FirebaseOpenSSLTransport::read(std::string* out) {
  char buf[kReadBufferSize];
  out->clear();
  http_parser_init(&parser_, HTTP_RESPONSE);
  settings_.on_body = [](http_parser* p, const char* at, size_t length)->int{
    auto s = reinterpret_cast<std::string*>(p->data);
    s->append(at, length);
  };
  for (;;) {
    int n = BIO_read(bio_, buf, sizeof(buf));
    if (n <= 0) {
      return ErrRead;
    }
    parser_.data = out;
    int nparsed = http_parser_execute(&parser_, &settings_, buf, n);
    if (nparsed <= 0) {
      return ErrParse;
    }
    if (http_body_is_final(&parser_)) {
      return nparsed;
    }
  }
}
