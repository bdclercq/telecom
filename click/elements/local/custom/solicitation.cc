#include <time.h>
#include <stdlib.h>
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <clicknet/ether.h>
#include "solicitation.hh"

CLICK_DECLS
Solicitation::Solicitation() : _timer(this), _consequent_messages(0)
{
}

Solicitation::~Solicitation()
{}

int Solicitation::configure(Vector<String> &conf, ErrorHandler *errh) {

if (Args(conf, this, errh)
    .read_mp("MNINFO", ElementCastArg("MNInfo"), _mninfo)
    .read_mp("SRCIP", _src_address)
	.read_mp("MAXRETRIES", _max_tries)
	.complete() < 0)
	return -1;

    _timer.initialize(this);
    _timer.schedule_after_msec(1000);
    return 0;
}

void Solicitation::run_timer(Timer* timer)
{
    // Indien de mobile node reeds verbonden is; doe niets
    // Indien er reeds advertisements zijn; doe niets
    if (_mninfo->_connected || !_mninfo->_advertisements.empty()) {
    
        _consequent_messages = 0;
        timer->schedule_after_msec(1000);
        return;
        
    }

    // Na een gespecifieerd aantal pogingen, stoppen we met een agent te vinden
    if (_consequent_messages >= _max_tries) {
    
        timer->schedule_after_msec(1000);
        return;
        
    }

    _consequent_messages++;

    int packetsize = sizeof(click_ip) + sizeof(solicitation_h);
    int headroom = sizeof(click_ether);
    
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);
    memset(packet->data(), 0, packet->length());


    //solicitations hebben een ttl van 1!
    click_ip* iph = (click_ip*)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = 5;
    iph->ip_tos = 0;
    iph->ip_len = htons(packetsize);
    iph->ip_ttl = 1;
    iph->ip_p = 1;
    iph->ip_src = _src_address;
    iph->ip_dst.s_addr = 0xffffffff;

    packet->set_dst_ip_anno(iph->ip_dst);

    solicitation_h* ash = (solicitation_h*)(iph + 1);
    ash->type = 10;
    ash->code = 0;

    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), sizeof(click_ip));
    ash->checksum = click_in_cksum((unsigned char*)ash, packetsize - sizeof(click_ip));

    output(0).push(packet);

    timer->schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Solicitation)
