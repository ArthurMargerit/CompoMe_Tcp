#include "Links/CompoMe/Posix/Tcp_client_out/Tcp_client_out.hpp"

namespace CompoMe {

namespace Posix {

//============================= addr =============================
CompoMe::String Tcp_client_out::get_addr() const { return this->addr; }

void Tcp_client_out::set_addr(const CompoMe::String p_addr) {
  this->addr = p_addr;
}

CompoMe::String &Tcp_client_out::a_addr() { return this->addr; }
//--------------------------------------------------------------------------
//============================= port =============================
i32 Tcp_client_out::get_port() const { return this->port; }

void Tcp_client_out::set_port(const i32 p_port) { this->port = p_port; }

i32 &Tcp_client_out::a_port() { return this->port; }
//--------------------------------------------------------------------------
//============================= to_component =============================
CompoMe::String Tcp_client_out::get_to_component() const {
  return this->to_component;
}

void Tcp_client_out::set_to_component(const CompoMe::String p_to_component) {
  this->to_component = p_to_component;
}

CompoMe::String &Tcp_client_out::a_to_component() { return this->to_component; }
//--------------------------------------------------------------------------
//============================= to_interface =============================
CompoMe::String Tcp_client_out::get_to_interface() const {
  return this->to_interface;
}

void Tcp_client_out::set_to_interface(const CompoMe::String p_to_interface) {
  this->to_interface = p_to_interface;
}

CompoMe::String &Tcp_client_out::a_to_interface() { return this->to_interface; }
//--------------------------------------------------------------------------

// Get Port /////////////////////////////////////////////////////////////

CompoMe::Stream::out &Tcp_client_out::get_main() { return this->main; }
CompoMe::Stream::map_out &Tcp_client_out::get_out() { return this->out; }

} // namespace Posix

} // namespace CompoMe
