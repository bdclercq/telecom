#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/args.hh>
#include "homeagent.hh"

CLICK_DECLS
Homeagent::Homeagent()
{}

Homeagent::~ Homeagent()
{}

int Homeagent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh).read_mp("SRC", _src).read_mp("DST", _dst).complete() < 0) return -1;
	return 0;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(Homeagent)
