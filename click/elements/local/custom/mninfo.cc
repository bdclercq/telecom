#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>

#include "mninfo.hh"

CLICK_DECLS

MNInfo::MNInfo() : _connected(false) {}
MNInfo::~MNInfo() {}

int MNInfo::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("HOMEAGENT", _home_agent)
        .read_mp("HOMEADDRES", _home_address)
        .complete() < 0)
        return -1;
        
    _foreign_agent = IPAddress(0);
    return 0;
        

}

CLICK_ENDDECLS

EXPORT_ELEMENT(MNInfo)
