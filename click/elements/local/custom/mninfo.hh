#ifndef CLICK_MNINFO_HH
#define CLICK_MNINFO_HH

#include <click/element.hh>

CLICK_DECLS

class MNInfo : public Element {

    public:
        MNInfo();
        ~MNInfo();
        
        const char *class_name() const { return "MNInfo"; }
        const char *port_count() const { return "0/0"; }
        
        int configure(Vector<String>&, ErrorHandler*);
        
        IPAddress _home_agent;
        IPAddress _home_address;
        
        bool _connected;
        IPAddress _foreign_agent;
        uint16_t _lifetime;

};

CLICK_ENDDECLS

#endif
