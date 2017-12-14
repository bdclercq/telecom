// Home or Foreign Agent
// The input/output configuration is as follows:
//
// Input:
//	[0]: packets received on the private network
//	[1]: packets received on the public network
//
// Output:
//	[0]: packets sent to the private network
//	[1]: packets sent to the public network
//	[2]: packets destined for the router itself

elementclass Agent {
	$private_address, $public_address, $gateway |
	
	Agent :: FA($public_address);
	
	Advertiser :: Advertisement($private_address, $public_address, false, true, false, 27, 100)

	// Shared IP input path and routing table
	ip :: Strip(14)
		-> CheckIPHeader
		
		//add ip classifer, if not port 434 or icmp: continue...
		-> regs::IPClassifier(src or dst udp port 434 or icmp type unreachable, -)[1]
		
		-> rt :: StaticIPLookup(
					$private_address:ip/32 0,
					$public_address:ip/32 0,
					$private_address:ipnet 1,
					$public_address:ipnet 2,
					0.0.0.0/0 $gateway 2);
	
	// ARP responses are copied to each ARPQuerier and the host.
	arpt :: Tee (2);
	
	// Input and output paths for interface 0
	input
	
	    //!!!check if solicitation, if not, continue...
	    -> checkSolicitation :: CheckSolicitation[1]
	    
		-> HostEtherFilter($private_address)
		-> private_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($private_address)
		-> output;

	private_arpq :: ARPQuerier($private_address)
		-> output;

	private_class[1]
		-> arpt
		-> [1]private_arpq;

	private_class[2]
		-> Paint(1)
		-> ip;
		
    //!!! if solicitation, sent advertisement...
    checkSolicitation[0] -> Advertiser

	// Input and output paths for interface 1
	input[1]
		-> HostEtherFilter($public_address)
		-> public_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($public_address)
		-> [1]output;

	public_arpq :: ARPQuerier($public_address)
		-> [1]output;

	public_class[1]
		-> arpt[1]
		-> [1]public_arpq;

	public_class[2]
		-> Paint(2)
		-> ip;

	// Local delivery
	rt[0]
		-> [2]output
		
		
    //if not port 434 or icmp
    regs[0]
        -> Unstrip(14)
        -> relay :: Relay(Agent, $private_address)[0]
        -> MarkIPHeader(0)
        -> SetIPChecksum
        -> [0]private_arpq
        
    relay[1]
        -> Strip(14)
        -> SetIPChecksum
        -> rt
        
	
	// Forwarding paths per interface
	rt[1]
		-> DropBroadcasts
		-> private_paint :: PaintTee(1)
		-> private_ipgw :: IPGWOptions($private_address)
		-> FixIPSrc($private_address)
		-> private_ttl :: DecIPTTL
		-> private_frag :: IPFragmenter(1500)
		-> private_arpq;
	
	private_paint[1]
		-> ICMPError($private_address, redirect, host)
		-> rt;

	private_ipgw[1]
		-> ICMPError($private_address, parameterproblem)
		-> rt;

	private_ttl[1]
		-> ICMPError($private_address, timeexceeded)
		-> rt;

	private_frag[1]
		-> ICMPError($private_address, unreachable, needfrag)
		-> rt;
	

	rt[2]
		-> DropBroadcasts
		-> public_paint :: PaintTee(2)
		-> public_ipgw :: IPGWOptions($public_address)
		-> FixIPSrc($public_address)
		-> public_ttl :: DecIPTTL
		-> public_frag :: IPFragmenter(1500)
		-> public_arpq;
	
	public_paint[1]
		-> ICMPError($public_address, redirect, host)
		-> rt;

	public_ipgw[1]
		-> ICMPError($public_address, parameterproblem)
		-> rt;

	public_ttl[1]
		-> ICMPError($public_address, timeexceeded)
		-> rt;

	public_frag[1]
		-> ICMPError($public_address, unreachable, needfrag)
		-> rt;
}
