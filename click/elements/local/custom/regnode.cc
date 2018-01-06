#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/udp.h>
#include <click/args.hh>

#include "RegRep.hh"
#include "regnode.hh"

CLICK_DECLS

RegNode::RegNode() : _timer(this), _registrationAlmostOver(3) {}

RegNode::~RegNode() {}

int RegNode::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("MNINFO", ElementCastArg("MNInfo"), _mninfo)
        .read_mp("REGISTRATIONALMOSTOVER", _registrationAlmostOver)
        .complete() < 0)
        return -1;

    _timer.initialize(this);
    //_timer.schedule_after_msec(1000);
    return 0;
}

void RegNode::push(int, Packet* p) {

    //click_chatter("WE ARE REGISTEREING");

    click_ip* iph = (click_ip*) p->data();
    click_udp* udph = (click_udp*)(iph + 1);
    regrep_h* reph = (regrep_h*)(udph + 1);
    
    
    request* recentreq;
    int recentreqit;
    bool found = false;
    for (int i = 0; i < _mninfo->pending.size(); i++) {
    
        request* current = &_mninfo->pending[i];
        if (current->dest == iph->ip_src && current->port == ntohs(udph->uh_dport)) {
            
            found = true;
            recentreq = current;
            recentreqit = i;
            break;
        
        }
    }
    
    if(!found) {
    
        p->kill();
        return;
    
    }
    
    if ((click_in_cksum_pseudohdr(click_in_cksum((unsigned char*)udph, p->length() - sizeof(click_ip)), iph, p->length() - sizeof(click_ip)) != 0) 
     && (ntohs(udph->uh_sum) != 0)) {
    
        p->kill();
        _mninfo->pending.erase(_mninfo->pending.begin() + recentreqit);
        return;
    
    }
    
    int id1 = reph->identification;
    int id2 = recentreq->id;
    if(id1 != id2) {
        p->kill();
        return;
    }
    
    uint8_t code = reph->code;
    uint16_t codestring = reph->code;
    
    //click_chatter("WE CHECK THE CODE");
    
    if (code == 0 || code == 1) {
    
        //click_chatter("WE'RE IN");
    
        _timer.clear();
        
        if (iph->ip_src != _mninfo->_home_agent) {
        
            //click_chatter("EH...");
        
            _mninfo->_connected = true;
            _mninfo->_foreign_agent = recentreq->dest;
            
            uint16_t given_lifetime = ntohs(reph->lifetime);
            uint16_t req_lifetime = recentreq->req_lt;
            uint16_t dec_lifetime = req_lifetime - given_lifetime;
            uint16_t lifetime = recentreq->rem_lt - dec_lifetime;
            
            _mninfo->_lifetime = lifetime;
            
            //click_chatter("WE GOT THE ADVERTISEMENT, LET'S START THE TIMER");
            
            _timer.schedule_after_msec(1000);
        
        }
        
        else {
            //_mninfo->_connected = true;
        }
        
        _mninfo->pending.erase(_mninfo->pending.begin() + recentreqit);

    }
    
    else {
        
        _mninfo->pending.erase(_mninfo->pending.begin() + recentreqit);
    
    }
    
    p->kill();

}

void RegNode::run_timer(Timer* timer) {

    //click_chatter("REGNODE TIMER");
    //click_chatter(String(_mninfo->_lifetime).c_str());

    if (_mninfo->_lifetime > 0) {
    
        click_chatter(String(_mninfo->_lifetime).c_str());
    
        _mninfo->_lifetime--;
        
        if (_mninfo->_lifetime == _registrationAlmostOver) {
        
            Packet* p = _mninfo->_advertisements[_mninfo->_foreign_agent];
            if (p != 0)
                output(0).push(p->clone());
        }
    
        if (_mninfo->_lifetime == 0) {
            _mninfo->_connected = false;
        }
    }
    
    timer->reschedule_after_msec(1000);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(RegNode);
