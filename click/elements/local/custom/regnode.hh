#ifndef CLICK_REGNODE_HH
#define CLICK_REGNODE_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "mninfo.hh"

CLICK_DECLS

class RegNode: public Element {
    public:
        RegNode();
        ~RegNode();

        const char *class_name() const { return "RegNode"; }
        const char *port_count() const { return "1/1"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);
        void push(int, Packet*);

    private:
        void run_timer(Timer* timer);

        MNInfo *_mninfo;

        unsigned int _registrationAlmostOver;

        Timer _timer;
};

CLICK_ENDDECLS

#endif /*CLICK_REGNODE_HH*/
