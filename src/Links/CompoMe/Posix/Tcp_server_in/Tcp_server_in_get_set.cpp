#include "Links/CompoMe/Posix/Tcp_server_in/Tcp_server_in.hpp"

namespace CompoMe {

namespace Posix {

//============================= addr =============================
CompoMe::String Tcp_server_in::get_addr() const { return this->addr; }

void Tcp_server_in::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}

CompoMe::String &Tcp_server_in::a_addr() { return this->addr; }
//--------------------------------------------------------------------------
//============================= port =============================
i32 Tcp_server_in::get_port() const { return this->port; }

void Tcp_server_in::set_port(const i32 p_port) { this->port = p_port; }

i32 &Tcp_server_in::a_port() { return this->port; }
//--------------------------------------------------------------------------
//============================= max_client =============================
ui32 Tcp_server_in::get_max_client() const { return this->max_client; }

void Tcp_server_in::set_max_client(const ui32 p_max_client) {
  this->max_client = p_max_client;
}

ui32 &Tcp_server_in::a_max_client() { return this->max_client; }
//--------------------------------------------------------------------------
//============================= max_request_size =============================
ui32 Tcp_server_in::get_max_request_size() const {
  return this->max_request_size;
}

void Tcp_server_in::set_max_request_size(const ui32 p_max_request_size) {
  this->max_request_size = p_max_request_size;
}

ui32 &Tcp_server_in::a_max_request_size() { return this->max_request_size; }
//--------------------------------------------------------------------------

// Get Port /////////////////////////////////////////////////////////////

CompoMe::Stream::in &Tcp_server_in::get_main() { return this->main; }
CompoMe::Stream::map_in &Tcp_server_in::get_many() { return this->many; }

} // namespace Posix

} // namespace CompoMe
