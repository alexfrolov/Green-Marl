#ifndef __common_h__
#define __common_h__
#include "gm_graph.h"

enum GraphType {RMAT, Random};
struct Options {
	graphlib::rmat_config rmat_cfg;
	graphlib::random_config random_cfg;

	GraphType graph_type;
	int nsources;
	int scale;
	int K;
	uint64_t N;
	uint64_t M;
	bool strongscale;
	bool directed;

	Options() {
		// default values
		rmat_cfg.scale = 10;
		rmat_cfg.K = 16;
		random_cfg.scale = 10;
		random_cfg.K = 64;
		scale = 10;
		K = 16;
		strongscale = true;
		graph_type = RMAT;
		nsources = 10;
		directed = true;
	}
};

void parse_options(CkArgMsg *m, Options *opts) {
	//if (argc == 1) usage(argc, argv);
	int argc = m->argc;
  char **argv = m->argv;

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--rmat")) {
			opts->graph_type = RMAT; 
		}
		if (!strcmp(argv[i], "--random")) {
			opts->graph_type = Random; 
		}
		if (!strcmp(argv[i], "--scale") || !strcmp(argv[i], "-s")) {
			opts->scale = (int) atoi(argv[++i]);
		}
		if (!strcmp(argv[i], "--strongscale") || !strcmp(argv[i], "--strong")) {
			opts->strongscale = true;
		}
		if (!strcmp(argv[i], "--weakscale") || !strcmp(argv[i], "--weak")) {
			opts->strongscale = false;
		}
		if (!strcmp(argv[i], "--directed")) {
			opts->directed = true;
		}
		if (!strcmp(argv[i], "--undirected")) {
			opts->directed = false;
		}
		/*if (!strcmp(argv[i], "--verify") || !strcmp(argv[i], "-v")) {
			opts->verify = true;
		}*/
		/*if (!strcmp(argv[i], "--freeze")) {
			opts->freeze = true;
		}*/
		/*if (!strcmp(argv[i], "--freeze-after-graph-creation")) {
			opts->freeze_after_graph_creation = true;
		}*/
		/*if (!strcmp(argv[i], "--max-clique-size") || !strcmp(argv[i], "-c")) {
			opts->ssca.maxCliqueSize = (int) atoi(argv[++i]);
		}*/
		/*if (!strcmp(argv[i], "--radix") || !strcmp(argv[i], "-r")) {
			opts->R = (int) atoi(argv[++i]);
		}*/
	}

	if (opts->strongscale)
		opts->N = (1 << opts->scale);
	else
		opts->N = (1 << opts->scale) * CkNumPes();
	opts->M = opts->N * opts->K;

	//FIXME:
	opts->rmat_cfg.scale = opts->scale;
	opts->random_cfg.scale = opts->scale;
	opts->rmat_cfg.K = opts->K;
	opts->random_cfg.K = opts->K;
	opts->rmat_cfg.directed = opts->directed;
	opts->random_cfg.directed = opts->directed;

	//while(opts->freeze) {}
}


#endif
