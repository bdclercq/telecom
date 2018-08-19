#ifndef CLICK_MNINFO_HH
#define CLICK_MNINFO_HH

#include <click/element.hh>
#include <click/hashmap.hh>

CLICK_DECLS

struct request {

    IPAddress dest;
    IPAddress coaddress;
    
    int id = 0;
    uint16_t req_lt;
    uint16_t rem_lt;
    
    uint16_t port;
};

class MNInfo : public Element {

    public:
        MNInfo();
	    MNInfo(IPAddress,IPAddress,IPAddress);
        ~MNInfo();
        
        const char *class_name() const { return "MNInfo"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }
        
        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);
        
        IPAddress _home_agent;
        IPAddress _home_address;
        IPAddress _gateway;
        
        bool _connected;
        IPAddress _foreign_agent;
        uint16_t _lifetime;
        
        HashMap<IPAddress, Packet*> _advertisements;
        Vector<request> pending;

};

CLICK_ENDDECLS

#endif
