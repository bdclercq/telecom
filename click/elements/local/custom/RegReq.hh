#ifndef CLICK_REGREQ_HH
#define CLICK_REGREQ_HH

#include <click/element.hh>
#include <clicknet/udp.h>
#include <click/atomic.hh>
#include <click/ipaddress.hh>
#include <click/timer.hh>

#include "mninfo.hh"

CLICK_DECLS

struct regreq_h {

    uint8_t type;
    uint8_t flags;
    uint16_t lifetime;
    IPAddress home_address;
    IPAddress home_agent;
    IPAddress care_of_address;
    uint64_t identification;

};

class RegReq : public Element {

  private:
  MNInfo* _mninfo;
  Timer _timer;
  
  uint64_t _identification;
  
  uint16_t _lifetime;

 public:

  RegReq() CLICK_COLD;
  ~RegReq() CLICK_COLD;

  const char *class_name() const	{ return "RegReq"; }
  const char *port_count() const	{ return PORTS_1_1; }
  const char *processing() const	{ return PUSH; }
  const char *flags() const		{ return "A"; }

  int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

    void run_timer(Timer *);
  Packet* make_packet(IPAddress dest, uint16_t lifetime, IPAddress coaddress);

};

CLICK_ENDDECLS

#endif
