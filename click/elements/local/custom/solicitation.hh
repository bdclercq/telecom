#ifndef CLICK_SOLICITATION_HH
#define CLICK_SOLICITATION_HH
#include <click/element.hh>
#include <click/timer.hh>

#include "mn.hh"

CLICK_DECLS

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
        MN* _mn;
        IPAddress _srcIp;
        Timer _timer;

        unsigned int _messagesSendInRow;
        unsigned int _maxRetries = 5;
};

struct solicitation_header {
    uint8_t     type;           /* 0     Type = 10 (Agent Solicitation) */
    uint8_t     code;           /* 1     Code = 0 */
    uint16_t    checksum;       /* 2-3   Checksum */
    uint8_t     addresses;      /* 4-7   Reserved */
};

CLICK_ENDDECLS
#endif
