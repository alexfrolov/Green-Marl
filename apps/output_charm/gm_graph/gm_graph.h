#ifndef __gm_graph_h__
#define __gm_graph_h__

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>
#include <graph500/graph500_generator.hpp>

namespace graphlib {
	struct rmat_config;
	struct random_config;

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}

}

#include "gm_graph.decl.h"
#include "rmat_generator.h"
#include "random_generator.h"

#define CK_TEMPLATES_ONLY
#include "gm_graph.def.h"
#undef CK_TEMPLATES_ONLY


#endif
