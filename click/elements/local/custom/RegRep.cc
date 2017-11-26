#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <click/vector.cc>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/glue.hh>

#include "RegRep.hh"

CLICK_DECLS

RegReq::RegRep() {}

RegReq::~RegRep() {}

int RegRep::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
    .read_mp("HA", ElementCastArg("HA"), _homeagent)
	.complete() < 0)
	    return -1;

    return 0;
}

void RegRep::push(int, Packet *p) {

    //check if acceptable
        //react to return value
    
    //add info to homeagent
    
    //make packet

}

int RegRep::check_acceptability(Packet *packet) {

    

}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegRep)

