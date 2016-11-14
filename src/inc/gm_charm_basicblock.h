#ifndef GM_CHARM_BASICBLOCK
#define GM_CHARM_BASICBLOCK
#include "gm_gps_basicblock.h"

class gm_charm_basic_block : public gm_gps_basic_block {
public:
	gm_charm_basic_block(int _id, int _type = GM_GPS_BBTYPE_SEQ) :
		gm_gps_basic_block(_id, _type) {}
};

#endif
