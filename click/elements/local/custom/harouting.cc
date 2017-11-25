#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <clicknet/ether.h>
#include "harouting.hh"

CLICK_DECLS
HARouting::HARouting()
{}

HARouting::~HARouting()
{}

int HARouting::configure(Vector<String> &conf, ErrorHandler *errh) {

	IPAddress address;

	if (Args(conf, this, errh)
        .read_mp("HOMEAGENT", address)
        .complete() < 0)
        return -1;
	
	_ha = new HA(address);
        
    return 0;
}

void HARouting::push(int, Packet* packet) {

    for (unsigned int i = 0; i < _ha->mobility_bindings.size(); ++i)
    {
        if (_ha->mobility_bindings[i].address == ((click_ip*)packet->data())->ip_dst)
        {
            output(1).push(packet);
            return;
        }
    }

    output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(HARouting)
