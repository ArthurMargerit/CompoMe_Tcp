#include "Links/CompoMe/Posix/Tcp_client_out/Tcp_client_out.hpp"
#include "CompoMe/Log.hpp"
#include "Interfaces/Interface.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace CompoMe {

namespace Posix {

  Tcp_client_out::Tcp_client_out() : CompoMe::Link(), main(), out(), sockfd(-1), addr("127.0.0.1"), port(1500) {
  this->main.set_link(*this);
  this->out.set_link(*this);
}

Tcp_client_out::~Tcp_client_out() {}

void Tcp_client_out::step() { Link::step(); }

void Tcp_client_out::main_connect() {
  Link::main_connect();

  sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->get_port());
  addr.sin_addr.s_addr = inet_addr(this->get_addr().str.c_str());

  this->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->sockfd == -1) {
    C_ERROR_TAG("tcp,client", "Socket creation error: ", strerror(errno));
    return;
  }

  auto r = connect(this->sockfd, (struct sockaddr *)&addr, sizeof(addr));
  if (r == -1) {
    C_ERROR_TAG("tcp,client", "Connection error: ", strerror(errno));
    this->main_disconnect();
    return;
  }
}

void Tcp_client_out::main_disconnect() {
  Link::main_disconnect();

  if (this->sockfd != -1) {
    close(this->sockfd);
    this->sockfd = -1;
  }
}

// one connect
void Tcp_client_out::one_connect(CompoMe::Require_helper &p_r,
                                 CompoMe::String p_key) {
  auto &nc = this->fake_many[p_key];

  nc.fss.set_func_send([this, p_key](CompoMe::String_d &d) {

    auto r = send(this->sockfd, d.str.c_str(), d.str.size(), 0);
    if (r == -1) {
      C_ERROR_TAG("http,client,send", "Send Error : ", strerror(errno));
      this->main_disconnect();
      throw "connection Error";
    }

    return true;
  });

  nc.rss.set_func_recv([this](CompoMe::String_d &d) {
    char l_buffer[1024 + 2];
    auto e = read(this->sockfd, l_buffer, 1024);
    if (e == -1) {
      C_ERROR_TAG("http,client", "Receive error");
      this->main_disconnect();
      return false;
    }

    if (e == 0) {
      C_ERROR_TAG("http,client", "Socket close");
      this->main_disconnect();
      return false;
    }

    l_buffer[e] = ' ';
    l_buffer[e + 1] = '\0';

    std::string str(l_buffer);
    d.str = str;

    return true;
  });

  nc.f = p_r.fake_stream_it(nc.fss, nc.rss);
}

void Tcp_client_out::one_connect(CompoMe::Interface &p_i,
                                 CompoMe::String p_key) {}

// one disconnect
void Tcp_client_out::one_disconnect(CompoMe::Require_helper &p_r,
                                    CompoMe::String p_key) {}

void Tcp_client_out::one_disconnect(CompoMe::Interface &p_i,
                                    CompoMe::String p_key) {}

} // namespace Posix

} // namespace CompoMe
