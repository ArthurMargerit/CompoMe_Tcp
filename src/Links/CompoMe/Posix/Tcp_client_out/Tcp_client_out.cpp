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

Tcp_client_out::Tcp_client_out() : CompoMe::Link(), f(nullptr){}

Tcp_client_out::~Tcp_client_out() {}

void Tcp_client_out::step() { Link::step(); }

void Tcp_client_out::main_connect() { 
  Link::main_connect();

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
    this->main_disconnect();
    return;
  }

}

void Tcp_client_out::main_disconnect() { 
  Link::main_disconnect();
  if (this->sock != -1) {
    close(this->sock);
    this->sock = -1;

    delete this->f;
    this->f = nullptr;
  }
}

} // namespace Posix

} // namespace CompoMe
