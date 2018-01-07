// Mobile Node
// The input/output configuration is as follows:
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

// Input:
//	[0]: packets received from the network
//
// Output:
//	[0]: packets sent to the network
//	[1]: packets destined for the host itself


elementclass MobileNode {
	$address, $gateway, $home_agent |
	
	Info :: MNInfo($gateway, $address)

	// Shared IP input path
	ip :: Strip(14)
		-> CheckIPHeader
		
		//add ip classifer, if not port 434 or icmp: continue...
		-> regs::IPClassifier(src or dst udp port 434, -)[1]
		
		-> rt :: LinearIPLookup(
			$address:ip/32 0,
			$address:ipnet 1,
			0.0.0.0/0 $gateway 1)
		-> [1]output;
		
	regs[0]
	    //-> register node
		-> rn::RegNode(Info, 3)
	    -> requester :: RegReq(Info, 1800)

	rt[1]	
	    -> processAdvertisements :: ProcessAdvertisements(Info)[0]
	    -> DropBroadcasts
	    -> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> Info
		-> arpq :: ARPQuerier($address)
		-> [0]output;
		
	processAdvertisements[1]
	    -> requester
	    -> arpq

	ipgw[1]	-> ICMPError($address, parameterproblem)
		-> output;

	ttl[1]	-> ICMPError($address, timeexceeded)
		-> output;

	frag[1]	-> ICMPError($address, unreachable, needfrag)
		-> output;

	// incoming packets
	input	
	    -> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1]
		-> [1]arpq;

	in_cl[2]
	    -> Paint(1)
		-> ip;
		
	//solicitation
	Solicitation(Info, $address:ip, 5)
	    ->arpq;
}
