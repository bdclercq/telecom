#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>

#include "mn.hh"

CLICK_DECLS

MN::MN() : _connected(false) {}

MN::MN(IPAddress agent, IPAddress address): _connected(false){
	_home_agent = agent;
	_home_address = address;
}
MN::~MN() {}

int MN::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("HOMEAGENT", _home_agent)
        .read_mp("HOMEADDRES", _home_address)
        .complete() < 0)
        return -1;
        
    _foreign_agent = IPAddress(0);
    return 0;
        

}

CLICK_ENDDECLS

EXPORT_ELEMENT(MN)
