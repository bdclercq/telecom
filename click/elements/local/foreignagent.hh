#include "mobileagent.hh"
#ifndef CLICK_FOREIGNAGENT_HH
#define CLICK_FOREIGNAGENT_HH
#include <click/element.hh>
#include <utility>
CLICK_DECLS

class Foreignagent : public Element { 
	public:
		Foreignagent();
		~Foreignagent();
		
		const char *class_name() const	{ return "Foreignagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
		void agentDiscovery(){}
		void tunnelMessage(){}
		void registrationReply(){}
		void forward(){}
	private:
		struct in_addr _src;
		struct in_addr _dst;
		Vector<Mobileagent> mobileNodes;
		Vector<std::pair<Mobileagent, String>> visitorList;
};

CLICK_ENDDECLS
#endif
