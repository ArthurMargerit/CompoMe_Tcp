
#include "Links/CompoMe/Posix/Tcp_client_out/Tcp_client_out.hpp"
#include "CompoMe/Log.hpp"
#include "Interfaces/Interface.hpp"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
namespace cstd {
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
} // namespace cstd

namespace CompoMe {

namespace Posix {

Tcp_client_out::Tcp_client_out()
    : CompoMe::Link(), fss(*this), rsr(*this), f(nullptr) {}

Tcp_client_out::~Tcp_client_out() {}

void Tcp_client_out::step() { Link::step(); }

void Tcp_client_out::connect() {

  Link::connect();
  cstd::sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = cstd::htons(this->get_port());
  addr.sin_addr.s_addr = cstd::inet_addr(this->get_addr().str.c_str());

  this->sock = cstd::socket(AF_INET, cstd::SOCK_STREAM, cstd::IPPROTO_TCP);
  if (sock == -1) {
    C_ERROR_TAG("tcp,client", "Socket creation error: ", strerror(errno));
    return;
  }

  auto r =
      cstd::connect(this->sock, (struct cstd::sockaddr *)&addr, sizeof(addr));
  if (r == -1) {
    C_ERROR_TAG("tcp,client", "Connection error: ", strerror(errno));
    this->disconnect();
    return;
  }

  this->f = this->a_re->fake_stream_it(fss, rsr);
}

void Tcp_client_out::disconnect() {
  Link::disconnect();
  if (this->sock != -1) {
    close(this->sock);
    this->sock = -1;

    delete this->f;
    this->f = nullptr;
  }
}

// Get and set /////////////////////////////////////////////////////////////

CompoMe::String Tcp_client_out::get_addr() const { return this->addr; }

void Tcp_client_out::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}
i32 Tcp_client_out::get_port() const { return this->port; }

void Tcp_client_out::set_port(const i32 p_port) { this->port = p_port; }
CompoMe::String Tcp_client_out::get_to_component() const {
  return this->to_component;
}

void Tcp_client_out::set_to_component(const CompoMe::String p_to_component) {
  this->to_component = p_to_component;
}
CompoMe::String Tcp_client_out::get_to_interface() const {
  return this->to_interface;
}

void Tcp_client_out::set_to_interface(const CompoMe::String p_to_interface) {
  this->to_interface = p_to_interface;
}
namespace Tcp_client_out_ns {
// stream
Return_string_stream_recv::Return_string_stream_recv(Tcp_client_out &p_l)
    : CompoMe::Return_stream_recv(), a_l(p_l) {}

void Return_string_stream_recv::pull() {
  char l_buffer[1024 + 2];
  auto e = read(this->a_l.get_sock(), l_buffer, 1024);
  if (e == -1) {
    C_ERROR_TAG("tcp,client", "Receive error");
    this->a_l.disconnect();
    return;
  }

  if (e == 0) {
    C_ERROR_TAG("tcp,client", "Socket close");
    this->a_l.disconnect();
    return;
  }

  l_buffer[e] = ' ';
  l_buffer[e + 1] = '\0';
  C_DEBUG_TAG("tcp,client,recv", "answer: ", l_buffer);
  std::string str(l_buffer);
  this->a_ss.str(str);
}

void Return_string_stream_recv::end() { this->a_ss.str(""); }

Function_string_stream_send::Function_string_stream_send(Tcp_client_out &p_l)
    : a_l(p_l) {}

void Function_string_stream_send::start() {
  this->a_ss.str("");
  if (this->a_l.get_to_component().str != "") {
    this->a_ss << this->a_l.get_to_component().str << ".";
  }

  if (this->a_l.get_to_interface().str != "") {
    this->a_ss << this->a_l.get_to_interface().str << ".";
  }
}

void Function_string_stream_send::send() {
  C_DEBUG_TAG("tcp,client,send", "call: ", this->a_ss.str());
  auto r = cstd::send(this->a_l.get_sock(), this->a_ss.str().c_str(),
                      this->a_ss.str().size(), 0);
  if (r == -1) {
    C_ERROR_TAG("tcp,client,send", "Send Error : ", strerror(errno));
    this->a_l.disconnect();
    throw "connection Error";
  }
}
} // namespace Tcp_client_out_ns
} // namespace Posix

} // namespace CompoMe
