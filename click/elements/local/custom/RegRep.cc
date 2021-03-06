#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <click/vector.cc>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/glue.hh>

#include "RegRep.hh"
#include "RegReq.hh"

#include <sstream>

CLICK_DECLS

RegRep::RegRep() {}

RegRep::~RegRep() {}

int RegRep::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
    .read_mp("HA", ElementCastArg("HA"), _home_agent)
	.complete() < 0)
	    return -1;

    return 0;
}

void RegRep::push(int, Packet *q) {

//    click_chatter("RegRep::push");

    click_ip* rip = (click_ip*)q->data();
    click_udp* rudp = (click_udp*)(rip + 1);
    regreq_h* req = (regreq_h*)(rudp + 1);

    //if the type is a request, go on...
    if (req->type == 1) {
//        click_chatter("RegRep request found");
        int acceptCode = check_acceptable(q);
        //click_chatter(String(acceptCode).c_str());
        if (acceptCode == 0 or acceptCode == 1) {
            //check if the home address is the co address, if so; remove the info that this node is on a foreign network
            if (req->home_address == req->care_of_address) {
//                click_chatter("COA == home address");
                for (int i = 0; i < _home_agent->_mobility_bindings.size();) {
                    if (_home_agent->_mobility_bindings[i].address == req->home_address) {
                        _home_agent->_mobility_bindings.erase(_home_agent->_mobility_bindings.begin() + i);
                        continue;
                    }
                    ++i;
                }
            }
            //else add it to the entries
            else {
                mobile_info mi;
                mi.address = IPAddress(req->home_address);
                mi.careOfAddress = IPAddress(req->care_of_address);
                mi.identification = req->identification;
                mi.remainingLifetime = req->lifetime;
                //click_chatter(IPAddress(req->home_address).unparse().c_str());
                //click_chatter(IPAddress(req->care_of_address).unparse().c_str());
                _home_agent->_mobility_bindings.push_back(mi);
            }
        }
        //make the reply
//        click_chatter("Create reply");
        int headroom = sizeof(click_ether) + 4;
        int p_size = sizeof(click_ip) + sizeof(click_udp) + sizeof(regrep_h);
        WritablePacket* p = Packet::make(headroom, 0, p_size, 0);
        
        if (!p)
            return;
            
        memset(p->data(), 0, p->length());
        
        //ip fields
        click_ip* ip = (click_ip*)p->data();
        ip->ip_v = 4;
        ip->ip_hl = 5;
        ip->ip_tos = 0;
        ip->ip_len = htons(p_size);
        ip->ip_ttl = 64;
        ip->ip_p = 17; //udp
        ip->ip_src = rip->ip_dst;
        ip->ip_dst = rip->ip_src;
        ip->ip_sum = click_in_cksum((unsigned char*) ip, sizeof(click_ip));
        
//        click_chatter(IPAddress(ip->ip_src).unparse().c_str());
        
        p->set_dst_ip_anno(ip->ip_dst);

        //udp fields
        click_udp* udp = (click_udp*)(ip + 1);
        udp->uh_sport = rudp->uh_dport;
        udp->uh_dport = rudp->uh_sport;
        
        uint16_t templ = p->length() - sizeof(click_ip);
        udp->uh_ulen = htons(templ);
        
        //reply fields
        regrep_h* rep = (regrep_h*)(udp + 1);
        rep->type = 3; //reply
        rep->code = acceptCode;
        rep->lifetime = req->lifetime;
        rep->home_address = req->home_address;
        rep->identification = req->identification;
        rep->home_agent = req->home_agent;
        
        udp->uh_sum = click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udp, p_size - sizeof(click_ip)), ip, p_size - sizeof(click_ip));
        
        if (req->home_address == req->care_of_address) {
            output(0).push(p);
//            click_chatter("Reply Locally");
        }
        else {
            output(1).push(p);
//            click_chatter("Reply Externally");
        } 
    }
   //if not a request... KILL
//   click_chatter("Kill cause not a request");
    q->kill();
}

int RegRep::check_acceptable(Packet *p) {

    click_ip* rip = (click_ip*)p->data();
    click_udp* rudp = (click_udp*)(rip + 1);
    regreq_h* req = (regreq_h*)(rudp + 1);
    
    uint8_t flags = req->flags;
    
    //X and R bit are always set as zero, return faulty if they are set
    if ((flags & 1) or (flags >> 2) & 1) {
        return 134;
    }
    
    if ((flags >> 5) & 1) {
        return 128;
    }
    
    int found = 0;
    
    //check if binding is already found...
    for (int i = 0; i < _home_agent->_mobility_bindings.size(); i++) {
        if (_home_agent->_mobility_bindings[i].address == req->home_address) {
            found = 1;
            break;
        }
    }
    
    //B bit should always be set to zero as broadcasting datagrams is not supported
    if (((flags >> 7) & 1) and (found == 1)) {
        return 135;
    }
    
    //broadcasting datagrams is not supported, thus the home agent address should not be 255.255.255.255
    if (req->home_agent == IPAddress("255.255.255.255")) {
        return 136;
    }
    return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegRep)

