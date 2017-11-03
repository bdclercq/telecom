#ifndef CLICK_AGENT_HH
#define CLICK_AGENT_HH
#include <click/element.hh>
CLICK_DECLS

class Agent : public Element { 
	public:
		Agent(){};
		~Agent(){};
		
		const char *class_name() const	{ return "Agent"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		virtual int configure(Vector<String>&, ErrorHandler*){};
		
		Packet* pull(int port){
			if (_queue.empty()) return 0;
			else {
				Packet *ret = _queue.front();
				_queue.pop_front();
				return ret;
			}
		};
	private:
		struct in_addr _src;
		struct in_addr _dst;
};

CLICK_ENDDECLS
#endif
