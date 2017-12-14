#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <clicknet/icmp.h>
#include <click/glue.hh>
#include <click/args.hh>

#include "relay.hh"
#include "RegReq.hh"
#include "RegRep.hh"

CLICK_DECLS

Relay::Relay() : _timer(this), _registrationLimit(-1) {}

Relay::~Relay() {}

int Relay::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
    .read_mp("FA", ElementCastArg("FA"), _fa)
    .read_mp("PRIVATEIP", _privateIP)
    .read_mp("REGLIMIT", _registrationLimit)
	.complete() < 0)
	    return -1;

    _timer.initialize(this);
    _timer.schedule_after_msec(1000);

    return 0;
}

void Relay::push(int, Packet* p) {

    click_ether* ethh = (click_ether*) p->data();
    click_ip* iph = (click_ip*) (ethh + 1);
    
    if (iph->ip_p == 1) {
        click_icmp* icmph = (click_icmp*)(iph + 1);
        click_ip* origip = (click_ip*)(icmph + 1);
        
        uint8_t type = icmph->icmp_type;
        
        if (type == 3) {
            IPAddress ha = IPAddress(origip->ip_dst);
            uint8_t code = icmph->icmp_code;
            uint8_t error;
            
            if (code == 0)
                error = 80;
            else if (code == 1)
                error = 81;
            else if (code == 3)
                error = 82;
            else
                error = 88;
                
            for (int i = 0; i < _fa->requests.size(); i++) {
            
                if (_fa->requests[i].home_agent == ha) {
                
                    IPAddress ipsrc = _fa->requests[i].ip_dst;
                    IPAddress ipdst = _fa->requests[i].ip_src;
                    uint16_t udpdst = _fa->requests[i].udp_src;
                    uint64_t id = _fa->requests[i].id;
                    
                    _fa->requests.erase(_fa->requests.begin() + i);
                    
                    Packet* pck = createRep(code, ipsrc, ipdst, udpdst, id, ha);
                    output(0).push(pck);
                    
                }
            }
        }
    }
    
    else {
    
        uint32_t p_size = p->length() - sizeof(click_ether);
        click_udp* udph = (click_udp*)(iph + 1);
        
        if (p_size == sizeof(click_ip) + sizeof(click_udp) + sizeof(regreq_h)) {
            regreq_h* req = (regreq_h*)(udph + 1);
            if (req->type == 1)
                relayReq(p);
        }
        
        else if (p_size == sizeof(click_ip) + sizeof(click_udp) + sizeof(regrep_h)) {
            regrep_h* rep = (regrep_h*)(udph + 1);
            if (rep->type == 1)
                relayRep(p);
        }
    }
    
    p->kill();

}

void Relay::run_timer(Timer* timer) {

    //remove registrations with lifetime < 0
    
    for (int i = 0; i < _fa->registrations.size(); i++) {
    
        if (_fa->registrations[i].second.remaining_lifetime > 0)
            _fa->registrations.erase(_fa->registrations.begin() + i);
    
    }

    //lower lifetime of registrations
    
    for (int i = 0; i < _fa->registrations.size(); i++) {
    
        if (_fa->registrations[i].second.remaining_lifetime > 0)
        _fa->registrations[i].second.remaining_lifetime;
    
    }
    
    //lower liftetime of requests
    
    for (int i = 0; i < _fa->requests.size(); i++) {
    
        vEntry* request = &_fa->requests[i];
        uint16_t lifetime = ntohs(request->remaining_lifetime);
        
        if (request->requested_lifetime > 0) {
            
            request->remaining_lifetime = htons(--lifetime);
            
            if (request->requested_lifetime - request->remaining_lifetime > 7) { //fa may delete request after more than 7 seconds
            
                //todo:: send timeout
                //_fa.requests.erase(_fa.requests.begin() + i);
            
            }
        }
        else
            _fa->requests.erase(_fa->requests.begin() + i);                       //delete if lifetime is under zero or lower...
    
    }
    
    timer->schedule_after_msec(1000);

}

void Relay::relayReq(Packet* p) {
    
    click_ether* ethh = (click_ether*) p->data();
    click_ip* iph = (click_ip*)(ethh + 1);
    click_udp* udph = (click_udp*)(iph + 1);
    regreq_h* reqh = (regreq_h*)(udph + 1);
    
    uint32_t psize = p->length() - sizeof(click_ether);
    
    //delete is udp checksum is wrong
    if ((click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, psize - sizeof(click_ip)), iph, psize - sizeof(click_ip)) != 0) && ntohs(udph->uh_sum) != 0) {
    
        p->kill();
        return;
    
    }
    
    uint8_t code;
    bool reject = false;
    
    //reject (66) registration limit is already met
    if (_registrationLimit > -1 && _fa->registrations.size() >= _registrationLimit){
    
        code = 66;
        reject = true;
    
    }
    
    uint8_t flags = reqh->flags;
    
    //reject (70) if flag 1 or flag 2 are 1 (which shouldn't happen)
    if ((flags & 1) || ((flags >> 2) & 1)) {
    
        code = 70;
        reject = true;
    
    }
    
    //reject (72) if flag 3 or flag 4 is 1
    if (((flags >> 3) & 1) || ((flags >> 4) & 1)) {
    
        code = 70;
        reject = true;
    
    }
    
    //send actual reject
    if (reject) {
    
        IPAddress ips = iph->ip_dst;
        IPAddress ipd = iph->ip_src;
        
        uint16_t udpd = udph->uh_sport;
        uint16_t id = reqh->identification;
        
        IPAddress ha = reqh->home_agent;
        
        Packet* pck = createRep(code, ips, ipd, udpd, id, ha);
        output(0).push(pck);
        
        p->kill();
        return;
    
    }
    
    //if not rejected, the request is now pending
    vEntry entry;
    entry.eth_src = EtherAddress(ethh->ether_shost);
    entry.ip_src = iph->ip_src;
    entry.ip_dst = iph->ip_dst;
    entry.udp_src = ntohs(udph->uh_sport);
    entry.home_agent = reqh->home_agent;
    entry.id = reqh->identification;
    entry.requested_lifetime = ntohs(reqh->lifetime);
    entry.remaining_lifetime = ntohs(reqh->lifetime);
    _fa->requests.push_back(entry);
    
    //relay request
    
    WritablePacket* pck = p->uniqueify();
    
    click_ether* rethh = (click_ether*) pck->data();
    click_ip* riph = (click_ip*)(rethh + 1);
    click_udp* rudph = (click_udp*)(riph + 1);
    
    riph->ip_len = htons(psize);
    riph->ip_ttl = 64;
    riph->ip_src = _fa->_address;
    riph->ip_dst = reqh->home_agent.in_addr();
    
    pck->set_dst_ip_anno(iph->ip_dst);
    
    rudph->uh_sport = htons(rand() % 65535);
    rudph->uh_dport = udph->uh_dport;
    rudph->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)rudph, psize - sizeof(click_ip)), riph, psize - sizeof(click_ip));
    
    output(1).push(pck);
    
}

void Relay::relayRep(Packet* p) {
    
    click_ether* ethh = (click_ether*) p->data();
    click_ip* iph = (click_ip*)(ethh + 1);
    click_udp* udph = (click_udp*)(iph + 1);
    regrep_h* reph = (regrep_h*)(udph + 1);
    
    uint32_t psize = p->length() - sizeof(click_ether);
    
    //delete is udp checksum is wrong
    if ((click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, psize - sizeof(click_ip)), iph, psize - sizeof(click_ip)) != 0) && ntohs(udph->uh_sum) != 0) {
    
        p->kill();
        return;
    
    }
    
    vEntry* entry;
    bool inlist = false;
    int iteration = 0;
    for (int i = 0; i < _fa->requests.size(); i++) {
    
        if (_fa->requests[i].ip_src == reph->home_address) {
            inlist = true;
            entry = & _fa->requests[i];
            iteration = i;
            break;    
        }
    }
    
    if (inlist) {
        p->kill();
        return;
    }
    
    //TODO::check if the identifications matches
    
    //check if accepted and do your thing
    uint8_t code = reph->code;
    if (code == 0 || code == 1) {
    
        uint16_t given_lifetime = ntohs(reph->lifetime);
        
        if (given_lifetime == 0){
        
            IPAddress home_addr = reph->home_address;
            
            int iter = 0;            
            for (int i = 0; i < _fa->registrations.size(); i++) {
            
                if (_fa->registrations[i].first == home_addr) {
                    iter = i;
                }
            }
            
            _fa->registrations.erase(_fa->registrations.begin() + iter);
        
        }
        else {
        
            IPAddress home_addr = reph->home_address;
            
            vEntry ent = *entry;
            ent.remaining_lifetime = reph->lifetime;
            std::pair<IPAddress, vEntry> newpair = std::make_pair(home_addr, ent);
        }
    }
    
    //delete from pending request
    
    _fa->requests.erase(_fa->requests.begin() + iteration);    
    
    //relay back
    
    WritablePacket* pck = p->uniqueify();
    
    click_ether* rethh = (click_ether*) pck->data();
    click_ip* riph = (click_ip*)(rethh + 1);
    click_udp* rudph = (click_udp*)(riph + 1);
    
    riph->ip_len = htons(psize);
    riph->ip_ttl = 64;
    riph->ip_src = _fa->_address;
    riph->ip_dst = reph->home_agent.in_addr();
    
    pck->set_dst_ip_anno(iph->ip_dst);
    
    rudph->uh_sport = htons(rand() % 65535);
    rudph->uh_dport = udph->uh_dport;
    rudph->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)rudph, psize - sizeof(click_ip)), riph, psize - sizeof(click_ip));
    
    pck->pull(14);
    output(1).push(pck);
}

Packet* Relay::createRep(uint8_t code, IPAddress ips, IPAddress ipd, uint16_t udpd, uint64_t id, IPAddress home_agent) {

    int headroom = sizeof(click_ether);
    int psize = sizeof(click_ip) + sizeof(click_udp) + sizeof(regrep_h);
    WritablePacket* p = Packet::make(headroom, 0, psize, 0);
    
    if (!p)
        return 0;
        
    memset(p->data(), 0, p->length());
    
    //ip fields
    click_ip* ip = (click_ip*)p->data();
    ip->ip_v = 4;
    ip->ip_hl = 5;
    ip->ip_tos = 0;
    ip->ip_len = htons(psize);
    ip->ip_ttl = 64;
    ip->ip_p = 17; //udp
    ip->ip_src = ips;
    ip->ip_dst = ipd;
    ip->ip_sum = click_in_cksum((unsigned char*) ip, sizeof(click_ip));
    
    p->set_dst_ip_anno(ipd);

    //udp fields
    click_udp* udp = (click_udp*)(ip + 1);
    udp->uh_sport = htons(434);
    udp->uh_dport = htons(udpd);
    udp->uh_ulen = htons(p->length() - sizeof(click_ip));
    
    //reply fields
    regrep_h* rep = (regrep_h*)(udp + 1);
    rep->type = 3; //reply
    rep->code = code;
    rep->lifetime = 0;
    rep->home_address = ipd;
    rep->identification = id;
    rep->home_agent = home_agent;
    
    udp->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udp, psize - sizeof(click_ip)), ip, psize - sizeof(click_ip));
    
    return p;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(Relay)
