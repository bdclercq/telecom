AddressInfo(sourceAddr 10.0.0.1 1A:7C:3E:90:78:41)
AddressInfo(responderAddr 10.0.0.2 1A:7C:3E:90:78:42)

info::MNInfo(10.0.0.2, 10.0.0.3);
requester::RegReq(info, 1800);
homeAgent::HA(20.0.0.1);
replyer::RegRep(homeAgent);

fakesource::ICMPPingSource(0.1.0.0, 0.2.0.0);
fakesource2::ICMPPingSource(0.3.0.0, 0.4.0.0);
copy::Tee(2);

fakesource2
    -> homeAgent

homeAgent[0]
    -> Discard
    
homeAgent[1]
    -> Discard

fakesource
    -> requester
    -> copy
    
copy[1]
    -> replyer[1]
    -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2)
    -> ToDump(reply.dump);
    
replyer[0]
    -> Discard
    
copy[0]
    -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2)
    -> ToDump(request.dump);
    
