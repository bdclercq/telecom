#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

#include "checksolicitation.hh"
#include "solicitation.hh"

CLICK_DECLS

CheckSolicitation::CheckSolicitation() {}

CheckSolicitation::~CheckSolicitation() {}

int CheckSolicitation::configure(Vector<String> &conf, ErrorHandler *errh) {

    //if (Args(conf, this, errh)
    //   .complete() < 0)
    //    return -1;

    return 0;
}

void CheckSolicitation::push(int, Packet* packet) {

    click_ether* ethh = (click_ether*)packet->data();
    click_ip* iph = (click_ip*)(ethh + 1);
    
    //if icmp
    if (iph->ip_p == 1) {
    
        solicitation_h* solh = (solicitation_h*)(iph + 1);
        
        //solicitation
        if (solh->type == 10) {
            
            if (iph->ip_src == 0) {
            
                packet->kill();
                return;
            
            }
            
            if (click_in_cksum((unsigned char*)solh, sizeof(solicitation_h)) != 0) {
            
                packet->kill();
                return;
            
            }
            
            if (solh->code != 0) {
            
                packet->kill();
                return;
            
            }
            
            if (ntohs(iph->ip_len) >= 8 + sizeof(click_ip)) {
            
                packet->kill();
                return;
            
            }
            
            output(0).push(packet);
            return;
            
        }
    }
    
    //if not icmp, not an solicitation
    output(1).push(packet);
    
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CheckSolicitation)
