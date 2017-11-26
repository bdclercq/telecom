#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <click/vector.cc>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/glue.hh>


#include "RegReq.hh"

CLICK_DECLS

RegReq::RegReq() : _timer(this), _lifetime(1800) {}

RegReq::~RegReq() {}

int RegReq::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
    .read_mp("MNINFO", ElementCastArg("MNInfo"), _mninfo)
    .read_mp("LIFETIME", _lifetime)
	.complete() < 0)
	    return -1;
	
    _timer.initialize(this);
    _timer.schedule_after_msec(1000);

    return 0;
}

Packet* RegReq::make_packet(IPAddress dest, uint16_t lifetime, IPAddress coaddress) {

    //dest is the src address of the advertised agent of the mn
    //lifetime is the lifetime advertised
    //coaddress is the coaddress give via the advertisement

    //add the request to pending request of mobile node, via mninfo
    request req;
    req.dest = dest;
    req.coaddress = coaddress;
    req.id = 0;
    req.req_lt = lifetime;
    req.rem_lt = lifetime;
    req.port = rand() % 65535;
    
    _mninfo->pending.push_back(req);

    int headroom = sizeof(click_udp);
    int p_size = sizeof(click_ip) + sizeof(regreq_h) + sizeof(regreq_h);
    WritablePacket* q = Packet::make(headroom, 0, p_size, 0);
    
    if (!q)
        return 0;
        
    memset(q->data(), '\0', q->length());
    
    //ip fields
    click_ip* ip = (click_ip*)q->data();
    ip->ip_v = 4;
    ip->ip_hl = 5;
    ip->ip_tos = 0;
    ip->ip_len = htons(p_size);
    ip->ip_ttl = 64;
    ip->ip_p = 17; //udp
    ip->ip_src = _mninfo->_home_address;
    ip->ip_dst = dest;
    ip->ip_sum = click_in_cksum((unsigned char*) ip, sizeof(click_ip));
    
    q->set_dst_ip_anno(ip->ip_dst);
    
    //udp fields
    click_udp* udp = (click_udp*)(ip + 1);
    udp->uh_sport = htons(req.port);
    udp->uh_dport = htons(434);
    udp->uh_ulen = htons(q->length() - sizeof(click_ip));
    
    //request fields
    regreq_h* rr = (regreq_h*)(udp + 1);
    
    rr->type = 1; //request
    
    
    uint8_t flagint = (0 << 7)  //hardcoded for now
                    + (0 << 6)  //broadcast datagrams is not supported
                    + (0 << 5)  //co-located care-of addresses are not supported
                    + (0 << 4)  //minimal encapsulation is not supported
                    + (0 << 3)  //gre encapsulation is not supported
                    + (0 << 2)  // sent as zero
                    + (0 << 1)  //reverse tunneling is not supported 
                    + (0);      //sent as zero
    
    rr->flags = flagint;
    rr->lifetime = htons(lifetime);
    rr->home_address = _mninfo->_home_address;
    rr->home_agent = _mninfo->_home_agent;
    rr->care_of_address = coaddress; //addressen zijn voorlopig hardcoded
    rr->identification = htons(0);
    
    return q;

}

void RegReq::run_timer(Timer *timer) {

    //todo:: take the information from the agent advertisement from the mninfo

    if (Packet *q = make_packet(IPAddress("0.0.0.0"), 1800, IPAddress("0.0.0.1"))) {
        output(0).push(q);
        _timer.reschedule_after_msec(1000);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegReq)

