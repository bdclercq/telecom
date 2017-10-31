#ifndef CLICK_MOBILEAGENT_HH
#define CLICK_MOBILEAGENT_HH
#include <click/element.hh>
CLICK_DECLS

class Mobileagent : public Element { 
	public:
		Mobileagent();
		~Mobileagent();
		
		const char *class_name() const	{ return "Homeagent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
	private:
		struct in_addr _src;
		struct in_addr _dst;
};

CLICK_ENDDECLS
#endif
