#ifndef CLICK_MN_HH
#define CLICK_MN_HH

#include <click/element.hh>
#include <click/hashmap.hh>

CLICK_DECLS

class MN : public Element {

    public:
        MN();
	    MN(IPAddress,IPAddress);
        ~MN();
        
        const char *class_name() const { return "MN"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }
        
        int configure(Vector<String>&, ErrorHandler*);
	    void push(int, Packet*);
        
        IPAddress _home_agent;
        IPAddress _home_address;
        
        bool _connected;
        IPAddress _foreign_agent;
        uint16_t _lifetime;

	    HashMap<IPAddress, Packet*> advertisements;

};

CLICK_ENDDECLS

#endif
