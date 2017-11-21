#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include "fainfo.hh"

CLICK_DECLS

FAInfo::FAInfo() {}
FAInfo::~FAInfo() {}

int FAInfo::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("ADDR", _address)
        .complete() < 0)
        return -1;
        
    return 0;
}

CLICK_DECLS

EXPORT_ELEMENT(FAInfo)
