#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "mobilenode.hh"

CLICK_DECLS
Mobilenode::Mobilenode()
{}

Mobilenode::~ Mobilenode()
{}

int Mobilenode::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

void Mobilenode::push(int, Packet *p){
	WritablePacket *q = p->uniqueify();
	click_ip *iph = (click_ip *)q->data();
	
	iph->ip_src = _srcIP.in_addr();
    iph->ip_ttl = 0;
	
	/// set the checksum afterwards using an element.
	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Mobilenode)
