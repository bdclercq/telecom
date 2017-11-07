#include "mobileagent.hh"
#ifndef CLICK_HOMEAGENT_HH
#define CLICK_HOMEAGENT_HH
#include <click/element.hh>
#include <utility>
CLICK_DECLS

class Homeagent : public Element { 
	public:
		Homeagent();
		~Homeagent();
		
		const char *class_name() const	{ return "Homeagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
		void agentDiscovery(){}
		void tunnelMessage(){}
		void registrationReply(){}
	private:
		struct in_addr _src;
		struct in_addr _dst;
		Vector<Mobileagent> mobileNodes;
		Vector<std::pair<Mobileagent, String>> bindings;
};

CLICK_ENDDECLS
#endif
