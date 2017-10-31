#ifndef CLICK_MOBILENODE_HH
#define CLICK_MOBILENODE_HH
#include <click/element.hh>
CLICK_DECLS

class Mobilenode : public Element { 
	public:
		Mobilenode();
		~Mobilenode();
		
		const char *class_name() const	{ return "Mobilenode"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
};

CLICK_ENDDECLS
#endif
