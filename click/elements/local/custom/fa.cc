#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include "fa.hh"

CLICK_DECLS

FA::FA() {}
FA::~FA() {}

int FA::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("ADDR", _address)
        .complete() < 0)
        return -1;
        
    return 0;
}

CLICK_DECLS

EXPORT_ELEMENT(FA)
