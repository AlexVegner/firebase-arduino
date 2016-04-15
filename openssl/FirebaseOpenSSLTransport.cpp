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
#include <unistd.h>

namespace {
const char kHTTPVersion[] = "HTTP/1.1";
const char kHostHeader[] = "Host: ";
const char kContentLengthHeader[] = "Content-Length: ";
const char kConnectionKeepAliveHeader[] = "Connection: keep-alive\r\n";
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
  BIO_set_nbio(bio_, 1); // TODO(proppy): switch to non blocking
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
  while(BIO_do_connect(bio_) <= 0) {} // TODO(proppy): use select
  int handshake_err = BIO_do_handshake(bio_);
  assert(handshake_err > 0);
  return 1;
}

int FirebaseOpenSSLTransport::write(const FirebaseGet& get) {
  int n = writeHeaders(get);
  assert(n >= 0);
  // TODO(proppy): add end-header to FirebaseRequest::raw to prevent
  // short-write
  int nn = BIO_puts(bio_, "\r\n"); // no body
  assert(nn >= 0);
  return readHeaders();
}

int FirebaseOpenSSLTransport::write(const FirebasePush& push) {
  return writeHeaders(push); // next write is body
}


int FirebaseOpenSSLTransport::write(const std::string& data) {
  std::string header(kContentLengthHeader);
  header += std::to_string(data.length());
  header += "\r\n\r\n"; // end request
  // TODO(proppy): proper write loop
  int n = BIO_puts(bio_, header.c_str()); // write header
  assert(n > 0);
  // TODO(proppy): proper write loop
  int nn = BIO_puts(bio_, data.c_str()); // write body
  assert(nn > 0);
  return readHeaders();
}

int FirebaseOpenSSLTransport::write(const FirebaseStream& stream) {
  int n = writeHeaders(stream);
  assert(n > 0);
  // TODO(proppy): add end-header to FirebaseRequest::raw to prevent
  // short-write
  int nn = BIO_puts(bio_, "\r\n"); // no body
  assert(n > 0);
  // handle redirect
  char buf[kReadBufferSize];
  http_parser_init(&parser_, HTTP_RESPONSE);
  //http_parser_settings_init(&settings_);
  std::string redirect;
  parser_.data = &redirect;
  settings_.on_header_field = [](http_parser* p, const char* at, size_t length)->int{
    printf("header_field %s\n", std::string{at, length}.c_str());
    std::string header{at, length};
    if (p->status_code == 307 && header == "Location") {
      reinterpret_cast<std::string*>(p->data)->assign(at, length);
    }
    return 0;
  };
  settings_.on_header_value = [](http_parser* p, const char* at, size_t length)->int{
    printf("header_value %s\n", std::string{at, length}.c_str());
    auto s = reinterpret_cast<std::string*>(p->data);
    if ((p->status_code == 307) && (*s == "Location")) {
      s->assign(at, length);
    }
    return 0;
  };
  settings_.on_headers_complete = [](http_parser* p)->int{
    return 1; // no body
  };
  for (;;) {
    int n;
    while((n = BIO_read(bio_, buf, sizeof(buf))) <= 0) {}
    assert(n > 0);
    printf("read: %s\n", buf);
    int nparsed = http_parser_execute(&parser_, &settings_, buf, n);
    printf("parsed: %d/%d\n", nparsed, n);
    assert(nparsed > 0);
    if (redirect.empty()) {
      streaming_ = true;
      return nparsed;
    }
    std::string host = redirect.substr(8, redirect.find_last_of("/")-8);
    std::string path = redirect.substr(redirect.find_last_of("/"));
    redirect = "";
    printf("host: %s, path %s\n", host.c_str(), path.c_str());
    int flush_err = BIO_flush(bio_);
    assert(flush_err >= 0);
    int reset_err = BIO_reset(bio_);
    assert(reset_err >= 0);
    int connect_err = connect(host.c_str());
    assert(connect_err > 0);
    std::string req = "GET " + path + " " + kHTTPVersion + "\r\n";
    req += kHostHeader + host + "\r\n";
    req += kConnectionKeepAliveHeader;
    req += "\r\n";
    printf("req: %s\n", req.c_str());
    int nn = BIO_puts(bio_, req.c_str());
    printf("puts: %d\n", nn);
    if (nn <= 0) {
      return ErrWrite;
    }
  }
}

int FirebaseOpenSSLTransport::writeHeaders(const FirebaseRequest& req) {
  streaming_ = false;
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

int FirebaseOpenSSLTransport::readHeaders() {
  char buf[kReadBufferSize];
  http_parser_init(&parser_, HTTP_RESPONSE);
  bool header_complete = false;
  parser_.data = &header_complete;
  settings_.on_headers_complete = [](http_parser* p)->int{
    auto b = reinterpret_cast<bool*>(p->data);
    *b = true;
    return 1;
  };
  for (;;) {
    int n;
    while((n = BIO_read(bio_, buf, sizeof(buf))) <= 0) {}
    printf("read: %d:%s\n", n, buf);
    int nparsed = http_parser_execute(&parser_, &settings_, buf, n);
    printf("parsed: %d:%d\n", nparsed, n);
    assert(nparsed > 0);
    if (header_complete) {
      body_length_left_ = parser_.content_length;
      if (parser_.status_code != 200) {
        return -parser_.status_code;
      }
      return parser_.status_code;
    }
  }
}

int FirebaseOpenSSLTransport::available() {
  if (streaming_) {
    // TODO(proppy): use select
    char buf[kReadBufferSize];
    BIO_read(bio_, buf, 0);
    return BIO_pending(bio_);
  }
  return body_length_left_;
}

int FirebaseOpenSSLTransport::read(std::string* out) {
  char buf[kReadBufferSize];
  int n = BIO_read(bio_, buf, sizeof(buf));
  assert(n > 0);
  printf("read: %d:%d\n", n, body_length_left_);
  if (!streaming_) {
    body_length_left_ -= n;
  }
  out->assign(buf, n);
  return n;
}
