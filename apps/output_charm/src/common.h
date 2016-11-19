#ifndef __common_h__
#define __common_h__
#include "gm_graph.h"

struct Options {
	graphlib::rmat_config rmat_cfg;
	int N;
	Options() {
		N = 16;
	}
};


void parse_options(CkArgMsg *m, Options *opts) {

}


#endif
