#include <clicknet/ip.h>
#include "RegReqEncap.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/standard/alignmentinfo.hh>
CLICK_DECLS

struct RegReqPacket {

    uint8_t type;
    uint8_t flags;
    uint16_t lifetime;
    uint32_t home_address;
    uint32_t home_agent;
    uint32_t care_of_address;
    uint64_t identification;

}

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
	.read_mp("S", _s_flag)
	.read_mp("B", _b_flag)
	.read_mp("D", _d_flag)
	.read_mp("M", _m_flag)
    .read_mp("G", _g_flag)
    .read_mp("r", _r_flag)
    .read_mp("T", _t_flag)
    .read_mp("x", _x_flag)
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

    int headroom = sizeof(click_ether) + sizeof(click_ip) + sizeof(click_udp);
    WritablePacket* q = Packet::make(headroom, 0, 160, 0);
    if (!q)
        return 0;
    memset(q->data(), '\0', 160);
    
    Vector<bool> flags = {_s_flag, _b_flag, _d_flag, _m_flag, _g_flag, _t_flag, _t_flag, _x_flag};
    
    RegReqPacket* rr = (RegReqPacket*)q->data();
    
    rr->type = _regreq_type;
    
    uint8_t flagint = 0;
    for (int i = 0; i < flags.size(); i++) {
        if (flags[i]) {
            flagint += pow(2, (flags.size() - 1) - i);
        }
    }
    
    rr->flags = flagint;
    rr->lifetime = _lifetime
    rr->home_address = _haddr;
    rr->home_agent = _haaddr;
    rr->care_of_address = _coaddr;
    rr->identification = _identicifation;
    
    return q;

}
