#ifndef __common_h__
#define __common_h__
#include "gm_graph.h"

struct Options {
	graphlib::rmat_config rmat_cfg;
	graphlib::random_config random_cfg;
	int N;
	Options() {
		// default values
		rmat_cfg.scale = 10;
		rmat_cfg.K = 16;
		random_cfg.scale = 10;
		random_cfg.K = 64;
		N = 16;
	}
};

void parse_options(CkArgMsg *m, Options *opts) {
	opts->N = (1 << opts->rmat_cfg.scale);
}


#endif
