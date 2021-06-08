#pragma once

#include "Data/CompoMe_Tcp.hpp"

#include "Links/Link.hpp"

// TYPES

#include "Data/CompoMe_Tcp.hpp"
#include "Types/CompoMe/String.hpp"

#include "Types/i32.hpp"
// STRUCT

// PORT

#include "Ports/CompoMe/Stream/out.hpp"

#include "Ports/CompoMe/Stream/map_out.hpp"

namespace CompoMe {

namespace Posix {

class Tcp_client_out : public CompoMe::Link {
public:
  Tcp_client_out();
  virtual ~Tcp_client_out();

  void step() override;
  void main_connect() override;
  void main_disconnect() override;

  // Get and set /////////////////////////////////////////////////////////////

  CompoMe::String get_addr() const;
  void set_addr(const CompoMe::String addr);
  CompoMe::String &a_addr();
  i32 get_port() const;
  void set_port(const i32 port);
  i32 &a_port();
  CompoMe::String get_to_component() const;
  void set_to_component(const CompoMe::String to_component);
  CompoMe::String &a_to_component();
  CompoMe::String get_to_interface() const;
  void set_to_interface(const CompoMe::String to_interface);
  CompoMe::String &a_to_interface();

  // Get Port /////////////////////////////////////////////////////////////

  CompoMe::Stream::out &get_main();
  CompoMe::Stream::map_out &get_out();

public:
  int get_sock() { return this->sock; }
  // Function
  // ///////////////////////////////////////////////////////////////////

private:
  int sock;
  CompoMe::Fake_stream *f;

  // DATA ////////////////////////////////////////////////////////////////////
  CompoMe::String addr;
  i32 port;
  CompoMe::String to_component;
  CompoMe::String to_interface;

  // PORT ////////////////////////////////////////////////////////////////////
  CompoMe::Stream::out main;
  CompoMe::Stream::map_out out;
};

} // namespace Posix

} // namespace CompoMe
