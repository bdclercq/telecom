#include <clicknet/ip.h>
#include "RegReqEncap.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/standard/alignmentinfo.hh>
CLICK_DECLS

RegReqEncap::RegReq()
{
}

RegReq::~RegReq()
{
}

int
RegReq::configure(Vector<String> &conf, ErrorHandler *errh)
{
    //bool do_cksum = true;
    //_interval = 0;
    if (Args(conf, this, errh)
	.read_mp("TYPE", _regreq_type)
	.read_mp("S", _flags[0])
	.read_mp("B", _flags[1])
	.read_mp("D", _flags[2])
	.read_mp("M", _flags[3])
    .read_mp("G", _flags[4])
    .read_mp("r", _flags[5])
    .read_mp("T", _flags[6])
    .read_mp("x", _flags[7])
    .read_mp("LIFETIME", _lifetime)
    .read_mp("HOME_ADDRESS", _haddr)
    .read_mp("HOME_AGENT", _haaddr)
    .read_mp("CARE_OF_ADDRESS", _coaddr)
    .read_mp("IDENTIFCATION", _identifcation)
	.complete() < 0)
	return -1;

  //_id = 0;
  //_cksum = do_cksum;
  //_count = 0;

#ifdef CLICK_LINUXMODULE
  // check alignment
  {
    int ans, c, o;
    ans = AlignmentInfo::query(this, 0, c, o);
    _aligned = (ans && c == 4 && o == 0);
    if (!_aligned)
      errh->warning("IP header unaligned, cannot use fast IP checksum");
    if (!ans)
      errh->message("(Try passing the configuration through `click-align'.)");
  }
#endif

  return 0;
}

Packet* RegReq::make_packet(){

    int headroom = sizeof(click_ether);
    WritablePacket* q = Packet::make(headroom, 0, 160, 0);
    if (!q)
        return 0;
    memset(q->data(), '\0', 160);
    
    //set specific bits to specific values
    
    q->set_dst_ip_anno(_daddr);

}
