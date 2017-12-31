#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <clicknet/icmp.h>
#include <click/glue.hh>
#include <click/args.hh>
#include "checkencap.hh"

CLICK_DECLS
CheckEncap::CheckEncap()
{}

CheckEncap::~CheckEncap()
{}

int CheckEncap::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh).complete() < 0) return -1;

    return 0;
}

void CheckEncap::push(int, Packet* packet) {

    click_ip* iph = (click_ip*)packet->data();
    if (iph->ip_p == 4)
        output(0).push(packet);
    else
        output(1).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CheckEncap)
