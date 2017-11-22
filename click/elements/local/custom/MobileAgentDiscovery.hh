//extends ICMP Router Discovery
//is formed by including an extension
//IP TTL must be 1
//used to determine current point of attachment
/*
other fields:
link layer destination address: same as source link-layer address 
IP destination: 255.255.255.255
ICMP fields: 
	code: 0 or 16
	lifetime
	router addresses
	num addrs

interval in which these advertisments are send shouldn't be longer than 1/3 of the lifetime given in the ICMP header
*/

#ifndef MOBILEAGENTDISCOVERY_HH
#define MOBILEAGENTDISCOVERY_HH
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class MobileAgentDiscovery : public Element {
    public:
        MobileAgentDiscovery();
        ~MobileAgentDiscovery();

        const char *class_name() const { return "MobileAgentDiscovery"; }
        const char *port_count() const { return "0/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);

    private:
	/*ICMP Router Discovery fields*/
	Vector<IPAddress> _routerIp;	//only own address(es)
	int _numAddrs;			//number of addresses advertised
	uint16_t _ICMPlifetime;
	/*Extension fields*/
        IPAddress _srcIp;
	IPAddress _dstIp;
        Timer _timer;
	uint8_t     _type = 16;
    	uint8_t     _length;
    	uint16_t    _seq_nr;
    	uint16_t    _lifetime;
    	uint8_t     _flags;	//RBHFMGrTUXI
    	uint8_t     _reserved;	//sent as zero
    	uint32_t    _address;	//number of addresses determined by length field
};

CLICK_ENDDECLS
#endif
