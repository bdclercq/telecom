#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include <time.h>
#include <stdlib.h>
#include <clicknet/ether.h>
#include "advertisement.hh"

CLICK_DECLS
Advertisement::Advertisement() : _interval(0), _timer(this), _sequenceNr(0){
    srand(time(NULL));
}

Advertisement::~Advertisement(){}

int Advertisement::configure(Vector<String> &conf, ErrorHandler *errh){
if (Args(conf, this, errh)
    .read_mp("SRCIP", _srcIp)
    .read_mp("COADDR", _careOfAddress)
	.read_mp("HA", _HA)
	.read_mp("FA", _FA)
	.read_mp("BUSY", _busy)
	.read_mp("LIFETIME", _advertisementLifetime)
	.complete() < 0)
	return -1;

    _timer.initialize(this);
    _timer.schedule_after_msec(1000);

    return 0;
}



void Advertisement::push(int, Packet* packet){
    click_ether* ethh = (click_ether*)packet->data();
    click_ip* iph = (click_ip*)(ethh + 1);

    sendPacket(iph->ip_src);
    packet->kill();
}

void Advertisement::run_timer(Timer *){
    sendPacket(IPAddress::make_broadcast());

    // Schedule the next advertisement
    _timer.schedule_after_msec((_interval * 1000) + ((rand() % 100) - 50));
}

void Advertisement::sendPacket(IPAddress destinationIP){

    int packetsize = sizeof(click_ip) + sizeof(advertisement_header) + sizeof(advertisement_header_extension);
    int headroom = sizeof(click_ether);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);

    memset(packet->data(), 0, packet->length());

    click_ip* iph = (click_ip*)packet->data();
    iph->ip_v = 4;
    iph->ip_hl = 5;
    iph->ip_tos = 0;
    iph->ip_len = htons(packetsize);
    iph->ip_id = htons(_sequenceNr);
    iph->ip_ttl = 1; // TTL must be 1 in advertisement
    iph->ip_p = 1; // protocol = ICMP
    iph->ip_src = _srcIp;
    iph->ip_dst = _srcIp;
    iph->ip_sum = click_in_cksum((unsigned char*)packet->data(), packet->length());

    packet->set_dst_ip_anno(iph->ip_dst);

    advertisement_header* advh = (advertisement_header*)(iph + 1);
	//rfc 1256: ICMP Router Discovery Messages --> type == 9 and code == 0
    advh->type = 9;
    advh->code = 0;
	//number of addresses advertised
    advh->addresses = 1;
	//The number of 32-bit words of information per each router address
    advh->addr_size = 2;
    advh->lifetime = htons(_advertisementLifetime);
    advh->address = _srcIp;
	//no preference needed, only one address per advertisement
    advh->preferences = 0;

    advertisement_header_extension* advhx = (advertisement_header_extension*)(advh + 1);
    advhx->type = 16;
    advhx->length = 6 + 4 * 1; // 6 bytes + 4 bytes for every ip address
    advhx->seq_nr = htons(_sequenceNr);
    advhx->lifetime = htons(_advertisementLifetime);
	//advertised care-of address provided by foreign agent
    advhx->address = _careOfAddress;
//    advhx->flags = {0,0,_HA, _FA, 0,0,0,0,0,0,0};
	advhx->flags.push_back(0);
	advhx->flags.push_back(_busy);
	advhx->flags.push_back(_HA);
	advhx->flags.push_back(_FA);
    for (int i =0;i<7;i++) advhx->flags.push_back(0);
    /*advhx->flags =  (0 << 7) // R flag (registration required)
                  + (_busy << 6) // Busy flag
                  + (_homeAgent << 5) // Home agent
                  + (_foreignAgent << 4) // Foreign agent
                  + (0 << 3) // M flag
                  + (0 << 2) // G flag
                  + (0 << 1) // r flag (sent as zero)
                  + 0;       // T flag
					+ // U flag
					+ // X flag
					+ // I flag*/
    advhx->reserved = 0;
    // Calculate the ICMP header checksum
    advh->checksum = click_in_cksum((unsigned char*)advh, packetsize - sizeof(click_ip));

    // Increment the sequence number
    if (_sequenceNr < 0xffff)
        _sequenceNr++;
    else
        _sequenceNr = 256;

    output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Advertisement)
