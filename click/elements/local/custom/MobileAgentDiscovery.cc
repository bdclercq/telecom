#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include "MobileAgentDiscovery.hh"

CLICK_DECLS
MobileAgentDiscovery::MobileAgentDiscovery()
{}

MobileAgentDiscovery::~ MobileAgentDiscovery()
{}

int MobileAgentDiscovery::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh).read_mp("ICMPLifeTime", _ICMPlifetime)
				.read_mp("Length", _length).complete() < 0) return -1;
	return 0;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileAgentDiscovery)
