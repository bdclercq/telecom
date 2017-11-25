#ifndef CLICK_HOMEAGENTMOBILENODEROUTING_HH
#define CLICK_HOMEAGENTMOBILENODEROUTING_HH
#include <click/element.hh>
#include <click/timer.hh>

#include "ha.hh"

CLICK_DECLS

class HARouting : public Element {
    public:
        HARouting();
        ~HARouting();

        const char *class_name() const { return "HARouting"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet*);

    private:
        HA* _ha;
};

CLICK_ENDDECLS
#endif
