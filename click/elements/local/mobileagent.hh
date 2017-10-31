#ifndef CLICK_MOBILEAGENT_HH
#define CLICK_MOBILEAGENT_HH
#include "agent.hh"
CLICK_DECLS

class Mobileagent : public Agent { 
	public:
		Mobileagent();
		~Mobileagent();
		
		const char *class_name() const	{ return "Homeagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*){};
};

CLICK_ENDDECLS
#endif
