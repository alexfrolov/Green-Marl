#ifndef CHARM_BEINFO_H
#define CHARM_BEINFO_H
#include "gm_gps_beinfo.h"

class gm_charm_beinfo : public gm_gps_beinfo {
public:
    gm_charm_beinfo(ast_procdef* d) : gm_gps_beinfo(d) {}
};

#endif
