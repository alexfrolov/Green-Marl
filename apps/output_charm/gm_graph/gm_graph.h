#ifndef __gm_graph_h__
#define __gm_graph_h__

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer/timer.hpp>
#include <graph500/graph500_generator.hpp>

namespace graphlib {
	struct rmat_config;
}

#include "gm_graph.decl.h"

namespace graphlib {

struct rmat_config {
	int scale;
	int K;
	void pup(PUP::er &p) {
		p | scale;
		p | K;
	}
};

template <typename Graph>
class rmat_generator : public CBase_rmat_generator <Graph> {
	public:
		rmat_generator() {}
		rmat_generator(Graph & g, struct rmat_config & cfg, bool strongscale) : 
			g(g), cfg(cfg), strongscale(strongscale) {}
		void generate();
		void addEdge(const std::pair<uint64_t, uint64_t> & e) {
			g.get_proxy()[e.first].add_edge(typename Graph::edge(e.second/*, w*/));
		}
	private:
		Graph g;
		struct rmat_config cfg;
		bool strongscale;
};

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}

template <typename Graph>
void rmat_generator<Graph>::generate() {
	CkPrintf("%d: generate\n", CmiMyPe());

	uint64_t seed64 = 12345;
	boost::rand48 gen, synch_gen;
	gen.seed(seed64);
	synch_gen.seed(seed64);
	srand48(12345);

	uint64_t N, M;

	if (strongscale)
		N = (1 << cfg.scale);
	else
		N = (1 << cfg.scale) * CkNumPes();
	M = N * cfg.K;


	boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
	uint64_t a = rand_64(gen);
	uint64_t b = rand_64(gen);

	uint64_t e_start = CkMyPe() * M / CkNumPes(); 
	//world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
	uint64_t e_count = M / CkNumPes(); 
	//(std::min)((m + world.size() - 1) / world.size(), m - e_start);

	boost::graph500_iterator graph500iter(
			strongscale == true ? cfg.scale : cfg.scale + __log2p2(CkNumPes()), 
			e_start, a, b);

	boost::timer::cpu_times tstart, tend;
	boost::timer::cpu_timer timer;

	tstart.clear();
	tstart = timer.elapsed();

	int thrash = 0;
	uint64_t ecounter = 0;

	for (uint64_t i = 0; i < e_count; i++) {
		if (((double)i/e_count)*100 > thrash) {
			//synchronize(pg);
			if (CkMyPe() == 0) {
				tend = timer.elapsed();
				CkPrintf("%d%%, %lld edges added, %0.2f sec\n", thrash, 
						ecounter, (double)(tend.wall - tstart.wall)/1000000000);
			}
			thrash+=10;
		}
		std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
		//add_edge(vertex(e.first, g), vertex(e.second, g), g);

		addEdge(e);

		graph500iter.increment();

		/*if (ecounter % YIELD_THRASHOLD)
			CthYield();*/
		ecounter++;
	}
}

template <typename Graph>
void create_rmat_graph(const Graph & g, struct rmat_config & cfg) {
	typedef CProxy_rmat_generator<Graph> CProxy_Generator;
 	CProxy_Generator generator = CProxy_Generator::ckNew(g, cfg, true);
	generator.generate();
}

};

#define CK_TEMPLATES_ONLY
#include "gm_graph.def.h"
#undef CK_TEMPLATES_ONLY


#endif
