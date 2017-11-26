#ifndef ADVERTISEMENT_HH
#define ADVERTISEMENT_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/vector.hh>

#include "mninfo.hh"

CLICK_DECLS

struct advertisement_h {
	//rfc 1256: ICMP Router Discovery Messages --> type == 9 and code == 0
    uint8_t     type;
    uint8_t     code;
    uint16_t    checksum;
	//number of addresses advertised
    uint8_t     addresses;
	//The number of 32-bit words of information per each router address (2, in the version of the protocol described here).
    uint8_t     addr_size;
    uint16_t    lifetime;
    in_addr     address;
	//no preference needed, only one address per advertisement
    uint32_t    preferences;
};

struct advertisement_h_e {
    uint8_t type ;
    uint8_t length;
    uint16_t seq_nr;
    uint16_t lifetime;
    uint8_t flags;
    uint8_t reserved;
	//advertised care-of address provided by foreign agent
    uint32_t address;
};

class Advertisement : public Element {
    public:
        Advertisement();
        ~Advertisement();

        const char *class_name() const { return "Advertisement"; }
        const char *port_count() const { return "0-1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
		void push(int, Packet*);
        void run_timer(Timer *);

	private:

		void sendPacket(IPAddress destinationIP);
		
        IPAddress _srcIp; //dst address must be the same as src that prompted the advertisement
        IPAddress _careOfAddress; //address used in extension only

		//1/3 of ICMP header lifetime
        double _interval;
        Timer _timer;

		//count of messages sent since initialization
        uint16_t _sequenceNr;

		//advertisement is for a HA or a FA
        bool _HA;
        bool _FA;
	    bool _busy;

        unsigned _advertisementLifetime;

        unsigned _ttl = 1;
};

CLICK_ENDDECLS
#endif
