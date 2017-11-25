#ifndef CLICK_SOLICITATION_HH
#define CLICK_SOLICITATION_HH
#include <click/element.hh>
#include <click/timer.hh>

#include "mn.hh"

CLICK_DECLS

struct solicitation_h {
    uint8_t     type;
    uint8_t     code;
    uint16_t    checksum;
    uint8_t     addresses;
};


class Solicitation : public Element {
    public:
        Solicitation();
        ~Solicitation();

        const char *class_name() const { return "Solicitation"; }
        const char *port_count() const { return "0/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);

        void run_timer(Timer*);

    private:
        MNInfo* _mninfo;
        IPAddress _src_address;
        Timer _timer;

        unsigned int _consequent_messages;
        unsigned int _max_try = 5;
};

CLICK_ENDDECLS
#endif
