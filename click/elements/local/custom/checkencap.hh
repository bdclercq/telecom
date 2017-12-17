#ifndef CLICK_CHECKIFENCAPSULATED_HH
#define CLICK_CHECKIFENCAPSULATED_HH
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class CheckEncap : public Element {
    public:
        CheckEncap();
        ~CheckEncap();

        const char *class_name() const { return "CheckEncap"; }
        const char *port_count() const { return "1/2"; }
        const char *processing() const { return PUSH; }

        int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet*);
};

CLICK_ENDDECLS
#endif
