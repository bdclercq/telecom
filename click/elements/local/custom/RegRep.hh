#ifndef CLICK_REGREP_HH
#define CLICK_REGREP_HH

#include <click/element.hh>
#include "ha.hh"

CLICK_DECLS

struct regrep_h {

    uint8_t type;
    uint8_t code;
    uint16_t lifetime;
    IPAddress home_address;
    IPAddress home_agent;
    uint64_t identification;

};

class RegRep : public Element {

    public:
    
    RegRep();
    ~RegRep();
    
    const char *class_name() const { return "RegRep"; }
    const char *port_count() const { return "1/2"; }
    const char *processing() const { return PUSH; }
    
    int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet*);
    
    private:
    
    HA* _home_agent;
    
    int check_acceptable(Packet* p);    

};

CLICK_ENDDECLS

#endif //CLICK_REGREP_HH
