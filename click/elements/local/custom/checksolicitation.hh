#ifndef CLICK_CHECKSOLICITATION_HH
#define CLICK_CHECKSOLICITATION_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class CheckSolicitation : public Element {

    public:
        CheckSolicitation();
        ~CheckSolicitation();

        const char *class_name() const { return "CheckSolicitation"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet*);

};

CLICK_ENDDECLS

#endif /*CLICK_CHECKSOLICITATION_HH*/
