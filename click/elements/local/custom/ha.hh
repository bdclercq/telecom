#ifndef HAINFO_HH
#define HAINFO_HH

#include <click/element.hh>
#include <click/etheraddress.hh>
#include <utility>

CLICK_DECLS

struct mobile_info{
	IPAddress address;
	IPAddress careOfAddress;
	uint64_t  identification=0;
	uint8_t   remainingLifetime;
};

class HA: public Element {
public:
	HA();
	HA(IPAddress);
	~HA();

	const char *class_name() const { return "HA"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);
	void push(int, Packet*);

	IPAddress _address;
	Vector<mobile_info> _mobility_bindings; //mobility binding list
};

CLICK_ENDDECLS

#endif
