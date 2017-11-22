#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <click/vector.cc>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/glue.hh>
#include <click/standard/alignmentinfo.hh>

#include "RegReq.hh"

CLICK_DECLS

RegReq::RegReq() : _timer(this), _lifetime(1800) {}

RegReq::~RegReq() {}

int RegReq::configure(Vector<String> &conf, ErrorHandler *errh)
{
	IPAddress home_agent;
        IPAddress home_address;
    if (Args(conf, this, errh)
    .read_mp("HOMAGENT", home_agent).read_mp("HOMEADDRESS", home_address)
    .read_mp("LIFETIME", _lifetime)
	.complete() < 0)
	return -1;

	_mn = new MN(home_agent, home_address);
	
    _timer.initialize(this);
    _timer.schedule_after_msec(1000);

    return 0;
}

Packet* RegReq::make_packet(){

    int headroom = sizeof(click_ether);
    int p_size = sizeof(RegReqHeader);
    WritablePacket* q = Packet::make(headroom, 0, p_size, 0);
    
    if (!q)
        return 0;
        
    memset(q->data(), '\0', q->length());
    
    RegReqHeader* rr = (RegReqHeader*)q->data();
    
    rr->type = 1;
    
    //all fixed for now
    uint8_t flagint = (0 << 7)
                    + (0 << 6)
                    + (0 << 5)
                    + (0 << 4)
                    + (0 << 3)
                    + (0 << 2)
                    + (0 << 1)
                    + (0);
    
    rr->flags = flagint;
    rr->lifetime = htons(_lifetime);
    rr->home_address = _mn->_home_address;
    rr->home_agent = _mn->_home_agent;
    rr->care_of_address = IPAddress(0); //fixed for now
    rr->identification = htons(0);
    
    return q;

}

void RegReq::run_timer(Timer *timer)
{
    if (Packet *q = make_packet()) {
        output(0).push(q);
        _timer.reschedule_after_msec(1000);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegReq)

