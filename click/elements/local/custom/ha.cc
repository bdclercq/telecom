#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include "ha.hh"

CLICK_DECLS

HA::HA() {}
HA::HA(IPAddress a){
	_address = a;
}
HA::~HA() {}

int HA::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("ADDR", _address)
        .complete() < 0)
        return -1;
        
    return 0;
}

void HA::push(int, Packet* packet) {

    for (unsigned int i = 0; i < _mobility_bindings.size(); ++i)
    {
        if (_mobility_bindings[i].address == ((click_ip*)packet->data())->ip_dst)
        {
            output(1).push(packet);
            return;
        }
    }
	//packet is not for a mobile node connected to this home agent
    output(0).push(packet);
}

CLICK_DECLS

EXPORT_ELEMENT(HA)
