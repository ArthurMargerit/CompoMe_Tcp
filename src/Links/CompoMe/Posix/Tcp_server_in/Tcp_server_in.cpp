#include "Links/CompoMe/Posix/Tcp_server_in/Tcp_server_in.hpp"
#include "CompoMe/Log.hpp"
#include "CompoMe/Tools/Call.hpp"
#include "Interfaces/Interface.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

namespace CompoMe {

namespace Posix {

Tcp_server_in::Tcp_server_in()
    : CompoMe::Link(), main(), many(), addr("127.0.0.1"), port(80), i_fds(0),
      fds(nullptr) {
  this->main.set_link(*this);
  this->many.set_link(*this);
}

Tcp_server_in::~Tcp_server_in() { this->main_disconnect(); }

void Tcp_server_in::step() {
  Link::step();

  int ret = poll(this->fds, this->i_fds, 0);

  if (ret == 0) {
    // C_DEBUG("Timeout Poll() on socket");
    return;
  }

  if (ret == -1) {
    C_ERROR("Error in Poll() on socket");
    return;
  }

  std::cout << "step"
            << "\n";

  // socket message
  // if it's the listening_socket manage the new connection
  if (this->fds[0].revents & POLLIN) {
    if (this->i_fds == this->get_max_client()) {
      auto socket_err = accept(this->fds[0].fd, nullptr, nullptr);
      write(socket_err, "!!!MAX!!! CONNECTION", 20);
      close(socket_err);
    } else {
      fds[this->i_fds].fd = accept(this->fds[0].fd, nullptr, nullptr);
      fds[this->i_fds].events = POLLIN | POLLHUP | POLLERR;
      fds[this->i_fds].revents = 0;

      fds[0].revents = fds[0].revents - POLLIN; // remove it
      this->i_fds++;
      C_INFO_TAG("tcp,server,recv", "new client");
    }
  }

  if (this->fds[0].revents & POLLHUP) {
    this->main_disconnect();
  }

  for (int i = 1; i < this->i_fds; i++) {

    if (this->fds[i].revents & POLLIN) {
      ssize_t readden =
          recv(this->fds[i].fd, buff, this->get_max_request_size(), 0);
      buff[readden] = '\0';

      C_INFO_TAG("tcp,sexrver,recv", "call: \"\"", buff, "\"");
      auto result_of_call = this->get_main().call(std::string(buff));

      C_INFO_TAG("tcp,server,recv", "res: \"", result_of_call.second, "\"");

      ssize_t r = 0;
      result_of_call.second.str += " "; // add a space to avoid empty message
      r = send(fds[i].fd, result_of_call.second.str.c_str(),
               result_of_call.second.str.length(), MSG_NOSIGNAL);
      if (r == -1) {
        C_ERROR_TAG("tcp,server,recv", "respond sending failled",
                    strerror(errno));
      }
    }

    if (fds[i].revents & POLLHUP) {
      // close socket
      close(this->fds[i].fd);
      this->fds[i].events = 0;
      this->fds[i].revents = 0;

      // swap it with the lastone
      this->fds[i].fd = this->fds[this->i_fds - 1].fd;
      this->fds[i].events = this->fds[this->i_fds - 1].events;
      this->fds[i].revents = this->fds[this->i_fds - 1].revents;

      this->i_fds--;
      i--;
    }

    if (this->fds[i].revents & POLLERR) {
      C_ERROR("Error in Poll()");
    }
  }
}

void Tcp_server_in::main_connect() {
  Link::main_connect();

  this->listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listening_socket == -1) {
    C_ERROR("socket() failed");
    return;
  }

  int enable = 1;
  setsockopt(this->listening_socket, SOL_SOCKET, SO_REUSEADDR, &enable,
             sizeof(int));

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->get_port());
  addr.sin_addr.s_addr = inet_addr(this->get_addr().str.c_str());
  {
    auto r = bind(listening_socket, (const struct sockaddr *)&addr,
                  sizeof(struct sockaddr));

    if (0 > r) {
      C_ERROR("bind at ", this->get_addr().str, ":", this->get_port(),
              " failed :", strerror(errno));
      return;
    }
  }

  {
    auto r = listen(this->listening_socket, 0);
    if (-1 == r) {
      C_ERROR("Listen failed : ", strerror(errno));
      return;
    }
  }

  {
    // +1 due to listening socket in this->fds[0]
    this->fds = (struct pollfd *)malloc((this->get_max_client() + 1) *
                                        sizeof(*this->fds));
    if (this->fds == nullptr) {
      C_ERROR("Malloc failed for fds size asked is ",
              (this->get_max_client() + 1) * sizeof(*this->fds));
      this->main_disconnect();
      return;
    }
  }

  {
    this->buff = (char *)malloc(sizeof(char) * this->get_max_request_size());
    if (this->buff == nullptr) {
      this->main_disconnect();
      C_ERROR("malloc failled for buff");
    }
  }

  {
    this->fds[0].fd = this->listening_socket;
    this->fds[0].events = POLLIN | POLLERR | POLLHUP;
    this->fds[0].revents = 0;
    this->i_fds = 1; // listening socket
  }
}

void Tcp_server_in::main_disconnect() {
  Link::main_disconnect();

  if (this->listening_socket != -1) {
    close(this->listening_socket);
    this->listening_socket = -1;
  }

  if (this->fds != nullptr) {
    for (int i = 1; i < this->i_fds; i++) {
      close(this->fds[i].fd);
    }
    free(this->fds);
    this->fds = nullptr;
  }

  free(this->buff);
  this->buff = nullptr;
}

// one connect
void Tcp_server_in::one_connect(CompoMe::Require_helper &p_r,
                                CompoMe::String p_key) {}

void Tcp_server_in::one_connect(CompoMe::Interface &p_i,
                                CompoMe::String p_key) {}

// one disconnect
void Tcp_server_in::one_disconnect(CompoMe::Require_helper &p_r,
                                   CompoMe::String p_key) {}

void Tcp_server_in::one_disconnect(CompoMe::Interface &p_i,
                                   CompoMe::String p_key) {}

} // namespace Posix

} // namespace CompoMe
