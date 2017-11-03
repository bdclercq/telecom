#ifndef CLICK_REGREQ_HH
#define CLICK_REGREQ_HH
#include <click/element.hh>
#include <click/glue.hh>
#include <click/atomic.hh>
#include <clicknet/udp.h>
#include <vector>
CLICK_DECLS

class RegReq : public Element {


  
  struct in_addr _saddr;
  struct in_addr _daddr;
  
  uint16_t _sport;
  uint16_t _dport;
  
  struct in_addr _haddr;
  struct in_addr _haaddr;
  struct in_addr _coaddr;
  
  uint64_t _identification;
  
  uint8_t _regreq_type;
  uint16_t _lifetime;
  
  vector<bool> _flags (8, 0);
  
  #ifdef CLICK_LINUXMODULE
  bool _aligned : 1;
  #endif

 public:

  RegReq() CLICK_COLD;
  ~RegReq() CLICK_COLD;

  const char *class_name() const	{ return "RegReq"; }
  const char *port_count() const	{ return PORTS_1_1; }
  const char *flags() const		{ return "A"; }

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

  Packet *make_packet(Packet *);

};

CLICK_ENDDECLS
#endif
