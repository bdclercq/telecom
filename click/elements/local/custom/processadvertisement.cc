#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/straccum.hh>
#include <click/args.hh>

#include "processadvertisement.hh"
#include "advertisement.hh"

CLICK_DECLS

ProcessAdvertisements::ProcessAdvertisements() {}

ProcessAdvertisements::~ProcessAdvertisements() {

    for (int i = 0; i < _mninfo->_advertisements.size(); i++) {
    
        //kill all packets in the advertisements of the mninfo
        for (HashMap<IPAddress, Packet*>::iterator i = _mninfo->_advertisements.begin(); i != _mninfo->_advertisements.end(); ++i)
        i.pair()->value->kill();
    
    }
}

int ProcessAdvertisements::configure(Vector<String> &conf, ErrorHandler *errh) {

    if (Args(conf, this, errh)
        .read_mp("MNINFO", ElementCastArg("MNInfo"), _mninfo)
        .complete() < 0)
        return -1;

    return 0;
}

void ProcessAdvertisements::push(int, Packet* packet) {

    //click_chatter("THIS MIGHT BE AN ADVERTISEMENT");

    click_ip* iph = (click_ip*) packet->data();
    
    if (iph->ip_p == 1) {
    
        //click_chatter("WHATEVER IT IS, ITS CERTAINLY AN ICMP");
    
        advertisement_h* ah = (advertisement_h*)(iph + 1);
        
        if (ah->type == 9) {
        
            //click_chatter("IT IS!!");
        
            uint16_t lsn = -1;
            advertisement_h_e* ahe = (advertisement_h_e*)(ah + 1);
            
            if (ahe->type == 16) {
            
                HashMap<IPAddress, Packet*>::Pair* p = _mninfo->_advertisements.find_pair(ah->address);
                if (p != 0) {
                
                    click_ip* ip_h = (click_ip*) p->value->data();
                    advertisement_h* a_h = (advertisement_h*)(ip_h + 1);
                    advertisement_h_e* a_he = (advertisement_h_e*)(a_h + 1);
                    
                    lsn = a_he->seq_nr;
                    
                    for (int i = 0; i < _timers.size(); i++) {
                    
                        if (_timers[i].first == p->key) {
                        
                            _timers.erase(_timers.begin() + i);
                            break;
                        
                        }
                    
                    }
                    
                    p->value->kill();
                    _mninfo->_advertisements.erase(ah->address);
                
                }
                
                if (((ahe->flags >> 6) & 1) == 0) {
                
                    if (!_mninfo->_connected) {
                    
                        _mninfo->_advertisements.insert(ah->address, packet->clone());
                        _latestRegAttempt.assign_now();
                        
                        //click_chatter("TIME TO SEND A REQUEST");
                        output(1).push(packet);
                    
                    }
                    
                    else if (ahe->seq_nr <= lsn && ahe->seq_nr < 256) {
                    
                        _mninfo->_advertisements.insert(ah->address, packet->clone());
                        _latestRegAttempt.assign_now();
                        
                        output(1).push(packet);
                    
                    }
                    
                    else {
                    
                        _mninfo->_advertisements.insert(ah->address, packet);
                    
                    }
                    
                    _timers.push_back(Pair<IPAddress, Timer>(ah->address, Timer(this)));
                    _timers.back().second.initialize(this);
                    _timers.back().second.schedule_after_msec(1000);
                
                }
                
                return;
            
            }
        }
    }
    
    output(0).push(packet);

}

void ProcessAdvertisements::run_timer(Timer* timer) {

    IPAddress address;
    Packet* packet;
    
    for (int i = 0; i < _timers.size(); i++) {
    
        if (&_timers[i].second == timer) {
        
            HashMap<IPAddress, Packet*>::Pair* p = _mninfo->_advertisements.find_pair(_timers[i].first);
            
            if (p == 0) {
            
                //no advertisement found
                return;
            
            }
            
            address = p->key;
            packet = p->value;
            break;
        
        }
    }
    
    click_ip* iph = (click_ip*) packet->data();
    advertisement_h* ah = (advertisement_h*)(iph + 1);
    
    uint16_t lifetime = ntohs(ah->lifetime);
    bool agentAvailable = _mninfo->_connected;
    
    if (lifetime > 0) {
    
        lifetime--;
        ah->lifetime = htons(lifetime);
        timer->reschedule_after_msec(1000);
    
    }
    
    else {
    
        if ((agentAvailable) && (_mninfo->_foreign_agent == address))
            agentAvailable = false;
            
        HashMap<IPAddress, Packet*>::Pair* p = _mninfo->_advertisements.find_pair(address);
        
        if (p != 0) {
        
            //no advertisement found
            return;
        
        }
        
        p->value->kill();
        _mninfo->_advertisements.erase(address);
        _mninfo->_connected = false;
        
        for (int i = 0; i < _timers.size(); i++) {
        
            if (&_timers[i].second == timer) {
            
                _timers.erase(_timers.begin() + i);
                break;
            
            }
        }
    }
    
    if (!agentAvailable) {
    
        if ((!_mninfo->_advertisements.empty()) && (_latestRegAttempt + Timestamp::make_sec(1) < Timestamp::now())) {
        
        _latestRegAttempt.assign_now();
        
        output(1).push(_mninfo->_advertisements.begin().pair()->value->clone());
        
        }
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ProcessAdvertisements)
