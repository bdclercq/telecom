#include <time.h>
#include <stdlib.h>
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <clicknet/ether.h>
#include "solicitation.hh"

CLICK_DECLS
Solicitation::Solicitation() : _timer(this), _messagesSendInRow(0)
{
}

Solicitation::~Solicitation()
{}

int Solicitation::configure(Vector<String> &conf, ErrorHandler *errh) {

if (Args(conf, this, errh)
    .read_mp("MNINFO", ElementCastArg("MNInfo"), _mn)
    .read_mp("SRCIP", _srcIp)
	.read_mp("MAXRETRIES", _maxRetries)
	.complete() < 0)
	return -1;

    _timer.initialize(this);
    _timer.schedule_after_msec(1000);
    return 0;
}

void Solicitation::run_timer(Timer* timer)
{
    // Don't do anything when it isn't needed yet
    if (_mn->_connected || !_mn->advertisements.empty()){
        _messagesSendInRow = 0;
        timer->schedule_after_msec(1000);
        return;
    }

    // If no router responds after a number of messages than stop sending messages
    if (_messagesSendInRow >= _maxRetries){
        timer->schedule_after_msec(1000);
        return;
    }

    _messagesSendInRow++;

    int packetsize = sizeof(click_ip) + sizeof(solicitation_header);
    int headroom = sizeof(click_ether);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);

    memset(packet->data(), 0, packet->length());

    click_ip* iph = (click_ip*)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = 5;
    iph->ip_tos = 0;
    iph->ip_len = htons(packetsize);
    iph->ip_ttl = 1; // TTL must be 1 in solicitation
    iph->ip_p = 1; // protocol = ICMP
    iph->ip_src = _srcIp;
    iph->ip_dst.s_addr = 0xffffffff;

    packet->set_dst_ip_anno(iph->ip_dst);

    solicitation_header* ash = (solicitation_header*)(iph + 1);
    ash->type = 10; // Agent solicitation
    ash->code = 0;

    // Calculate the header checksums
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), sizeof(click_ip));
    ash->checksum = click_in_cksum((unsigned char*)ash, packetsize - sizeof(click_ip));

    output(0).push(packet);

    timer->schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Solicitation)
