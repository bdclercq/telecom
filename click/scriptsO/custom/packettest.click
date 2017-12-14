AddressInfo(sourceAddr 10.0.0.1 1A:7C:3E:90:78:41)
AddressInfo(responderAddr 10.0.0.2 1A:7C:3E:90:78:42)

osource::ICMPPingSource(sourceAddr, responderAddr, INTERVAL 0.2, LIMIT 20);
info::MNInfo(10.0.0.2, 10.0.0.3);
source::RegReq(info, 1800);
responder::ICMPPingResponder;
switch::ListenEtherSwitch
//udpencap::UDPIPEncap(sourceAddr, 434, responderAddr, 434);
copy::Tee(2);

elementclass Router { $src | 
	
	querier::ARPQuerier($src);
	responder::ARPResponder($src);
	arpclass::Classifier(12/0806 20/0001, 12/0806 20/0002, -); // queries, responses, data
	
	input [0]
		-> [0]querier;

	querier
	    //-> ToDump(switch.dump)
		-> [0]output;

	input [1]
		-> arpclass;
		
	arpclass[0] // queries
		-> responder
		-> [0]output; // to network
  	
	arpclass[1] // replies
		-> [1]querier;
	
	arpclass[2] // data
		-> filter::HostEtherFilter($src);
		
	filter[0]
		-> Strip(14)
		-> MarkIPHeader
		-> [1] output

	filter[1]
		-> Discard;
}

sourceRouter::Router(sourceAddr)

osource
    -> source
    //-> udpencap
	-> copy
	
switch[0]
	-> [1] sourceRouter [1]
	-> source

responder
	-> [0] responderRouter::Router(responderAddr) [0]
	-> [1] switch	
	
switch[1]
	-> [1] responderRouter [1]
	-> responder

switch[2]
	//-> ToDump(switch.dump)
	-> Discard

copy[0]
    -> [0] sourceRouter [0]
	-> [0] switch

copy[1]
    -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2)
    -> ToDump(switch.dump);

