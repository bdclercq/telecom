#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>

#include "mninfo.hh"

CLICK_DECLS

MNInfo::MNInfo() : _connected(false) {}

MNInfo::MNInfo(IPAddress agent, IPAddress address, IPAddress gateway): _connected(false){
	_home_agent = agent;
	_home_address = address;
	_gateway = gateway;
}
MNInfo::~MNInfo() {}

int MNInfo::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("HOMEAGENT", _home_agent)
        .read_mp("HOMEADDRES", _home_address)
        .read_mp("GATEWAY", _gateway)
        .complete() < 0)
        return -1;
        
    _foreign_agent = IPAddress(0);
    return 0;
        
}

void MNInfo::push(int, Packet* packet) {

    // Send the package to the correct location
    if (_connected)
        packet->set_dst_ip_anno(_foreign_agent);

    output(0).push(packet);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(MNInfo)
