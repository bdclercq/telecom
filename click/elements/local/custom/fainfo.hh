#ifndef FAINFO_HH
#define FAINFO_HH

#include <click/element.hh>
#include <click/etheraddress.hh>
#include <utility>

CLICK_DECLS

struct vEntry {
    EtherAddress eth_src; // link-layer source address of the mobile node
    IPAddress ip_src; // mobile node IP home address
    IPAddress ip_dst; // destination IP address
    uint16_t udp_src; // UDP source port
    IPAddress home_agent; // Home Agent address
    uint64_t id; // identification field
    uint16_t requested_lifetime; // requested registration lifetime
    uint16_t remaining_lifetime; // remaining lifetime
};

class FAInfo: public Element {
public:
    FAInfo();
    ~FAInfo();

    const char *class_name() const { return "FAInfo"; }
    const char *port_count() const { return "0/0"; }

    int configure(Vector<String>&, ErrorHandler*);

	//visitor list entry per registration
    Vector<vEntry> requests;
	//list of mobile nodes
    Vector<std::pair<IPAddress, vEntry>> registrations;
    IPAddress _address;
};

CLICK_ENDDECLS

#endif
