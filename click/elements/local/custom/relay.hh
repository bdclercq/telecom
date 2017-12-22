#ifndef CLICK_RELAY_HH
#define CLICK_RELAY_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/atomic.hh>
#include <click/ipaddress.hh>
#include "fa.hh"

CLICK_DECLS

class Relay : public Element {

    public:
        Relay();
        ~Relay();
        
        const char *class_name() const { return "Relay"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }
        
        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        void run_timer(Timer*);
        
    private:
        FA* _fa;
        IPAddress _privateIP;
        Timer _timer;
        int _registrationLimit;
        
        void relayReq(Packet* p);
        void relayRep(Packet* p);
        Packet* createRep(uint8_t code, IPAddress ips, IPAddress ipd, uint16_t udpd, uint64_t id, IPAddress home_agent);
        

};

CLICK_ENDDECLS

#endif /*CLICK_RELAY_HH*/
