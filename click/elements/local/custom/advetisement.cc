#include <time.h>
#include <stdlib.h>
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include "advertisement.hh"

CLICK_DECLS
Advertisement::Advertisement() : _interval(0), _timer(this), _sequenceNr(0){
    srand(time(NULL));
}

Advertisement::~Advertisement(){}

int Advertisement::configure(Vector<String> &conf, ErrorHandler *errh){

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
    if (_maxAdvertisementInterval > _minAdvertisementInterval)
        _timer.schedule_after_msec((rand() % ((_maxAdvertisementInterval - _minAdvertisementInterval) * 1000)) + _minAdvertisementInterval * 1000);
    else // minimum = maximum
        _timer.schedule_after_msec((_minAdvertisementInterval * 1000) + ((rand() % 100) - 50));
}

void Advertisement::sendPacket(IPAddress destinationIP){
    int packetsize = sizeof(click_ip) + sizeof(advertisement_header) + sizeof(mobile_advertisement_header);
    int headroom = sizeof(click_ether);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, 0);
    if (packet == 0)
    {
        click_chatter("cannot make packet!");
        return;
    }

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
    iph->ip_dst = destinationIP;
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
    advh->addrPreference = 0;

    advertisement_header_extension* advhx = (advertisement_header_extension*)(advh + 1);
    advhx->type = 16;
    advhx->length = 6 + 4 * 1; // 6 bytes + 4 bytes for every ip address
    advhx->seq_nr = htons(_sequenceNr);
    advhx->lifetime = htons(_registrationLifetime);
	//advertised care-of address provided by foreign agent
    advhx->address = _careOfAddress;
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
    advhx->checksum = click_in_cksum((unsigned char*)advh, packetsize - sizeof(click_ip));

    // Increment the sequence number
    if (_sequenceNr < 0xffff)
        _sequenceNr++;
    else
        _sequenceNr = 256;

    output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Advertisement)
