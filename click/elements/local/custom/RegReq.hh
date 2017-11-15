#ifndef CLICK_REGREQ_HH
#define CLICK_REGREQ_HH

#include <click/element.hh>
#include <clicknet/udp.h>
#include <click/atomic.hh>
#include <click/ipaddress.hh>
#include <click/timer.hh>

CLICK_DECLS

class RegReq : public Element {

  private:
  IPAddress _saddr;
  IPAddress _daddr;
  
  uint16_t _sport;
  uint16_t _dport;
  
  IPAddress _haddr;
  IPAddress _haaddr;
  IPAddress _coaddr;
  
  uint64_t _identification;
  
  uint8_t _regreq_type;
  uint16_t _lifetime;
  
  bool _s_flag;
  bool _b_flag;
  bool _d_flag;
  bool _m_flag;
  bool _g_flag;
  bool _r_flag;
  bool _t_flag;
  bool _x_flag;
  
  #ifdef CLICK_LINUXMODULE
  bool _aligned : 1;
  #endif

 public:

  RegReq() CLICK_COLD;
  ~RegReq() CLICK_COLD;

  const char *class_name() const	{ return "RegReq"; }
  const char *port_count() const	{ return PORTS_1_1; }
  const char *processing() const	{ return PUSH; }
  const char *flags() const		{ return "A"; }

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  void push(Timer *);
  Packet *make_packet();

};
CLICK_ENDDECLS

#endif
