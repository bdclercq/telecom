#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <clicknet/ether.h>

#include <time.h>
#include <stdlib.h>

#include "advertisement.hh"

CLICK_DECLS
Advertisement::Advertisement() : _interval(0), _timer(this), _sequenceNr(0) {
    srand(time(NULL));
}

Advertisement::~Advertisement() {}

int Advertisement::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
    .read_mp("SRCIP", _srcIp)
    .read_mp("COADDR", _careOfAddress)
	.read_mp("HA", _HA)
	.read_mp("FA", _FA)
	.read_mp("BUSY", _busy)
	.read_mp("ALIFETIME", _advertisementLifetime)
	.read_mp("RLIFETIME", _registrationLifetime)
	.complete() < 0)
	    return -1;

    _timer.initialize(this);
    _timer.schedule_after_msec(1000);

    return 0;
}



void Advertisement::push(int, Packet* packet) {

    click_ether* ethh = (click_ether*)packet->data();
    click_ip* iph = (click_ip*)(ethh + 1);

    sendPacket(iph->ip_src);
    packet->kill();
}

void Advertisement::run_timer(Timer *) {

    sendPacket(IPAddress::make_broadcast());

    _timer.reschedule_after_msec((_interval * 1000) + ((rand() % 100) - 50));
}

void Advertisement::sendPacket(IPAddress destinationIP){

    int packetsize = sizeof(click_ip) + sizeof(advertisement_h) + sizeof(advertisement_h_e);
    int headroom = sizeof(click_ether);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);

    memset(packet->data(), 0, packet->length());

    //Advertisement TTL must be 1
    click_ip* iph = (click_ip*)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = 5;
    iph->ip_tos = 0;
    iph->ip_len = htons(packetsize);
    iph->ip_id = htons(_sequenceNr);
    iph->ip_ttl = 1;
    iph->ip_p = 1;
    iph->ip_src = _srcIp;
    iph->ip_dst = _srcIp;
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    packet->set_dst_ip_anno(iph->ip_dst);

    advertisement_h* advh = (advertisement_h*)(iph + 1);
    
	//rfc 1256: ICMP Router Discovery Messages --> type == 9 and code == 0
    advh->type = 9;
    advh->code = 0; 
    advh->addresses = 1; //number of addresses advertised
    advh->addr_size = 2; //The number of 32-bit words of information per each router address
    advh->lifetime = htons(_advertisementLifetime);
    advh->address = _srcIp; //no preference needed, only one address per advertisement
    advh->preferences = 0;

    advertisement_h_e* advhx = (advertisement_h_e*)(advh + 1);
    advhx->type = 16;
    advhx->length = 6 + 4 * 1; // 6 bytes + 4 bytes for every ip address, but only one address advertised
    advhx->seq_nr = htons(_sequenceNr);
    advhx->lifetime = htons(_registrationLifetime);
	
    advhx->address = _careOfAddress;// advertised care-of address provided by foreign agent
	
    advhx->flags =  (0 << 7)                // R flag (registration required)
                  + (_busy << 6)            // Busy flag
                  + (_HA << 5)              // Home agent
                  + (_FA << 4)              // Foreign agent
                  + (0 << 3)                // M flag, not supported, thus zero
                  + (0 << 2)                // G flag, not supported, thus zero
                  + (0 << 1)                // r flag (sent as zero)
                  + 0;                      // T flag, not supported, thus zero

    advhx->reserved = 0; //U, I, X flags are non supported, thus zero

    advh->checksum = click_in_cksum((unsigned char*)advh, packetsize - sizeof(click_ip));

    // 2.3.1
    if (_sequenceNr < 0xffff)
        _sequenceNr++;
    else
        _sequenceNr = 256;

    output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Advertisement)
