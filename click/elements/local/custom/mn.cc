#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>

#include "mn.hh"

CLICK_DECLS

MN::MN() {}

MN::~MN() {}

int MN::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("MNInfo", ElementCastArg("MNInfo"), _mn)
        .complete() < 0)
        return -1;
        
    return 0;
        

}

void MN::push(int, Packet* packet) {

    // Send the package to the correct location
    if (_mn->_connected)
        packet->set_dst_ip_anno(_mn->_foreign_agent);

    output(0).push(packet);
}

CLICK_ENDDECLS

EXPORT_ELEMENT(MN)
