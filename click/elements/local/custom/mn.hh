#ifndef CLICK_MN_HH
#define CLICK_MN_HH

#include <click/element.hh>
#include <click/hashmap.hh>

#include "mninfo.hh"

CLICK_DECLS

class MN : public Element {

    public:
        MN();
        ~MN();
        
        const char *class_name() const { return "MN"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }
        
        int configure(Vector<String>&, ErrorHandler*);
	    void push(int, Packet*);
	    
	    MNInfo* _mn;

	    HashMap<IPAddress, Packet*> advertisements;

};

CLICK_ENDDECLS

#endif
