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

class HAInfo: public Element {
public:
	HAInfo();
	~HAInfo();

	const char *class_name() const { return "HomeAgentInfobase"; }
	const char *port_count() const { return "0/0"; }

	int configure(Vector<String>&, ErrorHandler*);

	IPAddress _address;
	//mobility binding list
	Vector<mobile_info> mobility_bindings;
};

CLICK_ENDDECLS

#endif
