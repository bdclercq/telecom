#ifndef CLICK_MOBILEAGENT_HH
#define CLICK_MOBILEAGENT_HH
#include <click/element.hh>
#include <click/string.hh>
#include <click/vector.hh>
CLICK_DECLS
class Mobileagent : public Element { 
	public:
		Mobileagent();
		~Mobileagent();
		
		const char *class_name() const	{ return "Mobileagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
		void registrationRequest(){}
		void switchState(){}
		void detectMove(){}
		void returnHome(){}
		void chooseForeignAgent(){}
	private:
		struct in_addr _src;
		struct in_addr _dst;
		String multicast_group = "255.255.255.255";
		Vector<String> pendingRegistrations;
};

CLICK_ENDDECLS
#endif
