
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "mninfo.hh"

CLICK_DECLS

MNInfo::MNInfo() : connected(false) {}
MNInfo::~MNInfo() {}

int MNInfo::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .readMP("HOMEAGENT", _home_agent)
        .readMP("HOMEADDRES", _home_address)
        .complete() < 0)
        return -1;
        
    _foreign_agent = IPAddress(0);
    return 0;
        

}

CLICK_DECLS

EXPORT_ELEMENT(MNInfo)
