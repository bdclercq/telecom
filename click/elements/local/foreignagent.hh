#ifndef CLICK_FOREIGNAGENT_HH
#define CLICK_FOREIGNAGENT_HH
#include "agent.hh"
CLICK_DECLS

class Foreignagent : public Agent { 
	public:
		Foreignagent();
		~Foreignagent();
		
		const char *class_name() const	{ return "Homeagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*){};
};

CLICK_ENDDECLS
#endif
